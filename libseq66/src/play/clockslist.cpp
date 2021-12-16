/*
 *  This file is part of seq66.
 *
 *  seq66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  seq66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with seq66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          clockslist.cpp
 *
 *  This module defines some of the more complex functions of the clockslist.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2020-12-10
 * \updates       2021-12-16
 * \license       GNU GPLv2 or above
 *
 */

#include "play/clockslist.hpp"          /* seq66::clockslist class          */
#include "util/strfunctions.hpp"        /* seq66::string_format() template  */

/*
 *  This namespace is not documented because it screws up the document
 *  processing done by Doxygen.
 */

namespace seq66
{

/**
 *  Default and principal constructor defined in the header.
 */

/**
 *  Saves the clock settings read from the "rc" file so that they can be
 *  passed to the mastermidibus after it is created.  Also used in the
 *  creation of a port-map, in which the port nick-name is ultimately used to
 *  look up an index into the ports actually discovered in the system.
 *  The input string is of the following form (in ALSA), which includes the
 *  quotes:
 *
 *      1 0 "[1] 32:0 Launchpad Mini MIDI 1"
 *
 * \param buss
 *      The buss number read from the "rc" file.
 *
 * \param clocktype
 *      The clock value read from the "rc" file.
 *
 * \param name
 *      The full name of the port, except when a port-map is being formed.
 *      Then, this is just a string version of the buss number.  If this
 *      parameter is empty, nothing is added to the list.
 *
 * \param nickname
 *      The short name for the port, normally.  This is generally the text
 *      after the last colon in the bus/port name discovered by the system.
 *      By default, it is empty.
 *
 * \return
 *      Returns true if the item was added to the list.
 */

bool
clockslist::add
(
    int buss,
    e_clock clocktype,
    const std::string & name,
    const std::string & nickname,
    const std::string & alias
)
{
    bool result = buss >= 0 && ! name.empty();
    if (result)
    {
        std::string portname = next_quoted_string(name);
        if (portname.empty())                   /* was already parsed       */
            portname = name;

        io ioitem;
        ioitem.io_enabled = clocktype != e_clock::disabled;
        ioitem.out_clock = clocktype;
        ioitem.io_name = portname;
        ioitem.io_alias = alias;
        result = listsbase::add(buss, ioitem, nickname);
    }
    return result;
}

/**
 *  Sets a single clock item, if in the currently existing range.
 *  Mostly meant for use by the Options / MIDI Input tab and configuration
 *  files.
 */

bool
clockslist::set (bussbyte bus, e_clock clocktype)
{
    auto it = m_master_io.find(bus);
    bool result = it != m_master_io.end();
    if (result)
    {
        bool enabled = clocktype != e_clock::disabled;
        it->second.io_enabled = enabled;
        it->second.out_clock = clocktype;
    }
    return result;
}

e_clock
clockslist::get (bussbyte bus) const
{
    auto it = m_master_io.find(bus);
    return it != m_master_io.end() ? it->second.out_clock : e_clock::off ;
}

/*
 * Free functions
 */

clockslist &
output_port_map ()
{
    static clockslist s_clocks_list(true);      /* flag this as a port-map  */
    return s_clocks_list;
}

/**
 *  Gets the nominal port name for the given bus, from the internal port-map
 *  object for clocks.
 */

std::string
output_port_name (bussbyte b, bool addnumber)
{
    const clockslist & cloutref = output_port_map();
    return cloutref.get_name(b, addnumber);
}

/**
 *  Gets the port-string (e.g. "1") from the internal port-map object for
 *  clocks.
 */

bussbyte
output_port_number (bussbyte b)
{
    bussbyte result = b;
    const clockslist & cloutref = output_port_map();
    std::string nickname = cloutref.get_nick_name(b);
    if (! nickname.empty())
        result = std::stoi(nickname);

    return result;
}

/**
 *  Builds the internal clockslist which holds a simplified list of nominal
 *  outputs where the io_name field of each element is the nick-ndame of the
 *  source clockslist's element, and the io_nick_name field is the index
 *  number (starting from 0) converted to a string.
 */

bool
build_output_port_map (const clockslist & cl)
{
    bool result = cl.not_empty();
    if (result)
    {
        clockslist & cloutref = output_port_map();
        cloutref.clear();
        for (int b = 0; b < cl.count(); ++b)
        {
            bussbyte bb = bussbyte(b);
            std::string nick = cl.get_nick_name(bb);
            std::string number = std::to_string(b);
            std::string alias = cl.get_alias(bb);
            result = cloutref.add(b, e_clock::off, nick, number, alias);
            if (! result)
            {
                cloutref.clear();
                break;
            }
        }
        cloutref.active(result);
    }
    return result;
}

/**
 *  If an output map exists and is not empty [see the output_port_map()
 *  function], this function looks up the nominal buss number in order to find
 *  the registered (in the '[midi-clocks-map]' section of the 'rc' file) name
 *  of this port. That name is then used to look up the actual buss number of
 *  that port as set up by the system according to existing MIDI equipment.
 *
 * \param cl
 *      Provides the clockslist that holds the actual existing MIDI output
 *      ports.
 *
 * \param seqbuss
 *      Provides the 'virtual' (nominal) buss number to be mapped to the true
 *      buss number. The 'virtual' (nominal) buss number is the number stored
 *      with each pattern in the MIDI tune, and should never change just because
 *      the set of MIDI equipment changes.  In this manner, one can easily remap
 *      the configuration to fit the setup on someone else's system.
 *
 * \return
 *      If the port map exists, the looked-up port/buss number is returned. If
 *      that port cannot be found by name, then null_buss() (0xFF) is
 *      returned.  Otherwise, the nominal buss parameter is returned, which
 *      preserves the legacy behavior of the pattern buss number. Also,
 *      null_buss() will be returned if the nomimal buss is that value.
 */

bussbyte
true_output_bus (const clockslist & cl, bussbyte seqbuss)
{
    bussbyte result = seqbuss;
    if (! is_null_buss(result))
    {
        const clockslist & cloutref = output_port_map();
        if (cloutref.active())
        {
            std::string shortname = cloutref.port_name_from_bus(seqbuss);
            if (shortname.empty())
            {
                std::string msg = string_format("No output buss %d", seqbuss);
                errprint(msg);
                result = null_buss();
            }
            else
            {
#if defined USE_ALIAS_IF_PRESENT
                result = cl.bus_from_alias(shortname);
                if (is_null_buss(result))
                    result = cl.bus_from_nick_name(shortname);
#else
                result = cl.bus_from_nick_name(shortname);
#endif
                if (is_null_buss(result))
                {
                    const char * sn = shortname.c_str();
                    std::string msg = string_format
                    (
                        "No output buss %d (%s)", seqbuss, sn
                    );
                    errprint(msg);
                }
            }

        }
    }
    return result;
}

/**
 *  Returns a string representing the two columns of the internal clocks list.
 *  It is suitable for writing to a configuration file.  Quotes are included
 *  for readability and parse-ability.
 *
\verbatim
        0   "MIDI Port 1 Through"
        1   "Jazzy MIDI Out 1"
        2   "Jazzy MIDI Out 2"
\endverbatim
 *
 * \return
 *      Returns a string like the above.  If it is empty, the output port map
 *      is empty.
 */

std::string
output_port_map_list ()
{
    const clockslist & cloutref = output_port_map();
    return cloutref.port_map_list();
}

}               // namespace seq66

/*
 * clockslist.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

