#if ! defined SEQ66_USRSETTINGS_HPP
#define SEQ66_USRSETTINGS_HPP

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
 * \file          usrsettings.hpp
 *
 *  This module declares/defines just some of the global (gasp!) variables
 *  in this application.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2015-09-22
 * \updates       2021-06-09
 * \license       GNU GPLv2 or above
 *
 *  This module defines the following categories of "global" variables that
 *  are good to collect in one place, especially for settings stored in the
 *  "user" configuration file (<code> seq66.usr </code>):
 *
 *      -   The [user-midi-bus] settings, collected in the usermidibus
 *          class.
 *      -   The [use-instrument] settings, collected in the usrinstrument
 *          class.
 *      -   The [user-interface-settings] settings, a small collection of
 *          variables that describe some facets of the "Patterns Panel" or
 *          "Sequences Window", which is visually presented by the
 *          Gtk::Window-derived class called mainwnd.  These variables define
 *          the limits and resolution of various MIDI-to-GUI and application
 *          control parameters.
 *      -   The [user-midi-settings] settings, a collection of variables that
 *          will replaced hard-wired global MIDI parameters with modifiable
 *          parameters better suited to a range of MIDI files.
 *
 *  The Patterns Panel contains an 8-by-4 grid of "pattern boxes" or
 *  "sequence boxes".  All of the patterns in this grid comprise what is
 *  called a "set" (in the musical sense) or a "screen set".
 *
 *  We want to be able to change these defaults.  We will let you know when we
 *  are finished, and what you can do with these variables.
 */

#include <string>
#include <vector>

#include "cfg/basesettings.hpp"         /* seq66::basesettings class        */
#include "cfg/scales.hpp"               /* seq66::legal_key() and scale()   */
#include "cfg/userinstrument.hpp"
#include "cfg/usermidibus.hpp"

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  Provides the supported looping recording modes.  These values are used
 *  by the seqedit class, which provides a button with a popup menu to
 *  select one of these recording modes.
 */

enum class recordstyle
{
    merge,          /**< Incoming events are merged into the loop.  */
    overwrite,      /**< Incoming events overwrite the loop.        */
    expand,         /**< Incoming events increase size of loop.     */
    oneshot,        /**< Stop when length of loop is reached.       */
    max             /**< Provides an illegal/length value.          */
};

/**
 *  Holds the current values of sequence settings and settings that can
 *  modify the number of sequences and the configuration of the
 *  user-interface.  These settings will eventually be made part of the
 *  "user" settings file.
 */

class usrsettings final : public basesettings
{

    friend class cmdlineopts;   /* access for parse_o_options()             */
    friend class midifile;      /* allow access to midi_bpm_maximum()       */
    friend class usrfile;       /* allow protected access to file parser    */

private:

    /**
     *  Indicates what, if any, session manager will be used.
     */

    enum class session
    {
        none,
        nsm,
        lash,
        max
    };

    /**
     *  Provides a setting to control the overall style of grid-drawing for
     *  the pattern slots in mainwid.  These values can be specified in the
     *  [user-interface-settings] section of the "user" configuration file.
     *
     * \var normal
     *      The grid background color is the normal background color for the
     *      current GTK theme.  The box is drawn with brackets on either side.
     *
     * \var white
     *      The grid background color is white.  This style better fits
     *      displaying the white-on-black sequence numbers.  The box is drawn
     *      with brackets on either side.
     *
     * \var black
     *      The grid background color is black.
     *
     * \var button
     *      Indicates to use the live-grid rather than the live frame.
     *
     * \var max
     *      Marks the end of the list, and is an illegal value.
     */

    enum class grid
    {
        normal,
        white,
        black,
        button,
        max
    };

    /**
     *  [user-midi-bus-definitions]
     *
     *  Internal types for the container of usermidibus objects.
     *  Sorry about the "confusion" about "bus" versus "buss".
     *  See Google for arguments about it.
     */

    using Busses = std::vector<usermidibus>;

    /**
     *  [user-instrument-definitions]
     *
     *  Internal type for the container of userinstrument objects.
     */

    using Instruments = std::vector<userinstrument>;

    /**
     *  Provides data about the MIDI busses, readable from the "user"
     *  configuration file.  Since this object is a vector, its size is
     *  adjustable.
     */

    Busses m_midi_buses;

    /**
     *  Provides data about the MIDI instruments, readable from the "user"
     *  configuration file.  The size is adjustable, and grows as objects
     *  are added.
     */

    Instruments m_instruments;

    /**
     *  [user-interface-settings]
     *
     *  These are not labelled, but are present in the "user" configuration
     *  file in the following order:
     *
     *      -#  mainwnd-rows
     *      -#  mainwnd-cols
     *      -#  zoom
     *      -#  global-seq-feature
     *      -#  progress-bar-thick
     *      -#  window-redraw-rate-ms
     */

    /**
     *  Number of rows in the Patterns Panel.  The current value is 4, and if
     *  changed, many other values depend on it.  Together with
     *  m_mainwnd_cols, this value fixes the patterns grid into a 4 x 8 set of
     *  patterns known as a "screen set".  We would like to be able to change
     *  this value from 4 to 8, and maybe allow the values of 5, 6, and 7 as
     *  well.  But if we could just get 8 working, then well would Seq66
     *  deserve the 64 in its name, and it would also match the layouts of the
     *  Launchpad series of controllers.
     */

    int m_mainwnd_rows;

    /**
     *  Number of columns in the Patterns Panel.  The current value is 8, and
     *  probably won't change, since other values depend on it and it is a
     *  common grid size.  Together with m_mainwnd_rows, this value fixes the
     *  patterns grid into a 4 x 8 set of patterns known as a "screen set".
     */

    int m_mainwnd_cols;

    /**
     *  Provide a scale factor to increase the size of the main window
     *  and its internals.  Should be limited from 1.0 to 3.0, probably.
     *  Right now we allow 0.5 to 3.0 (c_window_scale_min to
     *  c_window_scale_default).  This value is used by the following
     *  functions:
     *
     *      -    window_scale()
     *      -    window_scaled_up()
     *      -    window_scaled_down()
     *      -    window_is_scaled()
     *      -    scale_size()
     */

    float m_window_scale;

    /**
     *  A new item to allow scaling window width and height separately.  If in
     *  the legal range, this item will scale the height.  Otherwise, the same
     *  value of m_window_scale will be used for both dimensions.
     */

    float m_window_scale_y;

    /**
     *  These control sizes.  We'll try changing them and see what
     *  happens.  Increasing these value spreads out the pattern grids a
     *  little bit and makes the Patterns panel slightly bigger.  Seems
     *  like it would be useful to make these values user-configurable.
     */

    int m_mainwid_spacing;  /* c_mainwid_spacing = 2; try 4 or 6 instead    */

    /**
     *  Provides the initial zoom value, in units of ticks per pixel.  The
     *  original default value was 32 ticks per pixel, but larger PPQN values
     *  need higher values, and we will have to adapt the default zoom to the
     *  PPQN value.  Also, the zoom can never be zero, as it can appear as the
     *  divisor in scaling equations.
     */

    int m_current_zoom;

    /**
     *  If true, this value provide a bit of backward-compatibility with the
     *  global key/scale/background-sequence persistence feature.  In this
     *  feature, applying one of these three changes to a sequence causes them
     *  to also be applied to sequences that are subsequently opened for
     *  editing.  However, we improve on this feature by allowing the changes
     *  to be saved in the global, proprietary part of the saved MIDI file.
     *
     *  If false, the user can still save the key/scale/background-sequence
     *  values with each individual sequence, so they can be different.
     *
     *  This value will be true by default, unless changed in the "user"
     *  configuration file.
     */

    bool m_global_seq_feature_save;

    /**
     *  Replaces seqedit::m_initial_scale as the repository for the scale to
     *  apply when a sequence is loaded into the sequence editor.  Its default
     *  value is scales::off.  Although this value is now stored in the
     *  usrsettings class, it always comes from the currently loaded MIDI
     *  file, if present.  If m_global_seq_feature_save is true, this variable
     *  is stored in the "proprietary" track at the end of the file, under the
     *  control tag c_musicscale, and will be applied to any sequence that is
     *  edited.  If m_global_seq_feature_save is false, this variable is
     *  stored, if used, in the meta-data for the sequence to which it
     *  applies, and, again, is tagged with the control tag c_musicscale.
     */

    int m_seqedit_scale;

    /**
     *  Replaces seqedit::m_initial_key as the repository for the key to apply
     *  when a sequence is loaded into the sequence editor.  Its default value
     *  is c_key_of_C.  Although this value is now stored in the usrsettings
     *  class, it always comes from the currently loaded MIDI file, if
     *  present.  If m_global_seq_feature_save is true, this variable is
     *  stored in the "proprietary" track at the end of the file, under the
     *  control tag c_musickey, and will be applied to any sequence that is
     *  edited.  If m_global_seq_feature_save is false, this variable is
     *  stored, if used, in the meta-data for the sequence to which it
     *  applies, and, again, is tagged with the control tag c_musickey.
     */

    int m_seqedit_key;

    /**
     *  The repository for the background sequence to apply when a sequence is
     *  loaded into the sequence editor.  Its default value is seq::limit().
     *  Although this value is now stored in the usrsettings class, it always
     *  comes from the currently loaded MIDI file, if present.  If
     *  m_global_seq_feature_save is true, this variable is stored, if it has
     *  a valid (but not "legal") value, in the "proprietary" track at the end
     *  of the file, under the control tag c_backsequence, and will be applied
     *  to any sequence that is edited.  If m_global_seq_feature_save is
     *  false, this variable is stored, if used, in the meta-data for the
     *  sequence to which it applies, and, again, is tagged with the control
     *  tag c_backsequence.
     */

    int m_seqedit_bgsequence;

    /**
     *  If set, makes progress bars thicker than 1 pixel... 2 pixels.
     *  It isn't useful to support anything thicker.  The default is true --
     *  use two pixels.
     */

    bool m_progress_bar_thick;

    /**
     *  If set, use an alternate, neo-inverse color palette.  Not all colors
     *  are reversed, though.
     */

    bool m_inverse_colors;

    /**
     *  Provides the global setting for redraw rate of windows.  Not all
     *  windows use this yet.  The default is 40 ms (c_redraw_ms, which is 25
     *  ms in Windows builds)), but some windows originally used 25 ms, so
     *  beware of side-effects.
     */

    int m_window_redraw_rate_ms;

    /**
     *  Constants for the mainwid class.  The m_seqchars_x and
     *  m_seqchars_y constants help define the "seqarea" size.  These look
     *  like the number of characters per line and the number of lines of
     *  characters, in a pattern/sequence box.
     *
     *  UNUSED!
     */

    int m_seqchars_x;   /* c_seqchars_x = 15    */
    int m_seqchars_y;   /* c_seqchars_y =  5    */

    /*
     *  [user-midi-settings]
     */

    /**
     *  Provides the default PPQN for the application.  This PPQN is used when
     *  creating a new MIDI file or when reading an existing file with the
     *  m_use_file_ppqn value set.  This value defaults to SEQ66_DEFAULT_PPQN
     *  = 192 (the legacy Seq24 value).
     */

    int m_default_ppqn;

    /**
     *  Provides the universal PPQN setting for the duration of this session.
     *  It is either the default PPQN or the MIDI file's PPQN.  The default
     *  value of this setting is 192 parts-per-quarter-note (PPQN).  There is
     *  still a lot of work to get a different PPQN to work properly in speed
     *  of playback, scaling of the user interface, and other issues.  Note
     *  that this value can be changed by the --ppqn option, as well as in the
     *  'rc' file.
     */

    int m_midi_ppqn;                     /* PPQN, parts per QN       */

    /**
     *  If true, ignore Seq66's default PPQN value and use the file's PPQN,
     *  leaving the file unscaled.
     */

    bool m_use_file_ppqn;

    /**
     *  Holds the PPQN read from the file, for use in file conversion if we're
     *  not using the file's PPQN.
     */

    int m_file_ppqn;

    /**
     *  Provides the universal and unambiguous MIDI value for beats per
     *  measure, also called "beats per bar" (BPB).  This variable will
     *  replace the global beats per measure.  The default value of this
     *  variable is SEQ66_DEFAULT_BEATS_PER_MEASURE (4).  For external access,
     *  we will call this value "beats per bar", abbreviate it "BPB", and use
     *  "bpb" in any accessor function names.  Now, although it applies to the
     *  whole session, we should be able to continue seq66's tradition of
     *  allowing each sequence to have its own time signature.  Also, there
     *  are a number of places where the number 4 appears and looks like it
     *  might be a hardwired BPB value, either for MIDI purposes or for
     *  drawing the piano-roll grids.  So we might need a couple different
     *  versions of this variable.
     */

    int m_midi_beats_per_measure;        /* BPB, or beats per bar       */

    /**
     *  Provides the minimum beats per minute, purely for providing the scale
     *  for drawing the tempo.  Defaults to 0.
     */

    midibpm m_midi_bpm_minimum;

    /**
     *  Provides the universal and unambiguous MIDI value for beats per minute
     *  (BPM).  This variable will replace the global beats per minute.  The
     *  default value of this variable is DEFAULT_BPM (120).  This variable
     *  should apply to the whole session; there's probably no way to support
     *  a diffent tempo for each sequence.  But we shall see.  For external
     *  access, we will call this value "beats per minute", abbreviate it
     *  "BPM", and use "bpm" in any accessor function names.
     */

    midibpm m_midi_beats_per_minute;        /* BPM, or beats per minute    */

    /**
     *  Provides the maximum beats per minute, purely for providing the scale
     *  for drawing the tempo.  Defaults to 127.
     */

    midibpm m_midi_bpm_maximum;

    /**
     *  Provides the universal MIDI value for beats width (BW).  This variable
     *  will replace the global beat_width.  The default value of this
     *  variable is DEFAULT_BEAT_WIDTH (4).  Now, although it applies to the
     *  whole session, we should be able to continue seq66's tradition of
     *  allowing each sequence to have its own time signature.  Also, there
     *  are a number of places where the number 4 appears and looks like it
     *  might be a hardwired BW value, either for MIDI purposes or for drawing
     *  the user-interface.  So we might need a couple different versions of
     *  this variable.  For external access, we will call this value "beat
     *  width", abbreviate it "BW", and use "bw" in any accessor function
     *  names.
     */

    int m_midi_beat_width;              /* BW, or beat width            */

    /**
     *  Provides a universal override of the buss number for all sequences, for
     *  the purpose of convenience of of testing.  This variable replaces the
     *  global buss-override variable, and is set via the command-line option
     *  --bus.
     */

    bussbyte m_midi_buss_override;      /* --bus n option               */

    /**
     *  Sets the default velocity for note adding.  The value
     *  SEQ66_PRESERVE_VELOCITY (-1) preserves the velocity of incoming notes,
     *  so that nuances in live playing can be preserved.  The popup-menu for
     *  the "Vol" button in the seqedit window shows this value as the "Free"
     *  menu entry.  The rest of the values in the menu show a few select
     *  velocities, but any velocity from 0 to 127 can be entered here. Of
     *  course, 0 is not recommended.
     */

    int m_velocity_override;

    /**
     *  Sets the precision of the BPM (beats-per-minute) setting.  The
     *  original value was effectively 0, but we need to be able to support
     *  the following values:
     *
     *      -   0.  The legacy default.
     *      -   1.  One decimal place in the BPM spinner.
     *      -   2.  Two decimal places in the BPM spinner.
     */

    int m_bpm_precision;

    /**
     *  The step increment value for BPM, regardless of the decimal precision.
     *  The default value is the legacy value, 1, for a BPM precision value of
     *  0.  The default value is 0.1 if one decimal place of precision is in
     *  force, and 0.01 if two decimal places of precision is in force.
     *  This is the increment that is performed in the BPM field of the main
     *  window when the arrow-buttons are clicked, the up/down arrow keys are
     *  pressed, or the BPM MIDI controls are processed.
     */

    midibpm m_bpm_step_increment;

    /**
     *  This is the larger increment for paging the BPM.  Currently, the only
     *  way to use this increment is to click in the BPM field of the main
     *  window and then use the Page-Up and Page-Down keys.
     */

    midibpm m_bpm_page_increment;

    /*
     *  Values calculated from other member values in the normalize() function.
     */

    /**
     *  The maximum number of patterns supported is given by the number of
     *  patterns supported in the panel (32) times the maximum number of
     *  sets (32), or 1024 patterns.  It is basically the same value as
     *  m_max_sequence by default.
     */

    int m_total_seqs;                   /* not included in .usr file    */

    /**
     *  Number of patterns/sequences in the Patterns Panel, also known as
     *  a "set" or "screen set".  This value is 4 x 8 = 32 by default.
     *
     * \warning
     *      Currently implicit/explicit in a number of the "rc" file and
     *      rcsettings.  Would probably want the left 32 or the first 32
     *      items in the main window only to be subject to keystroke control.
     *      This value is calculated by the normalize() function, and is <i>
     *      not </i> part of the "user" configuration file.
     */

    int m_seqs_in_set;                  /* not include in .usr file     */

    /**
     *  Number of group-mute tracks/sequences/patterns that can be supported,
     *  which is m_seqs_in_set squared, or 1024.  This value is <i> not </i>
     *  part of the "user" configuration file; it is calculated by the
     *  normalize() function.
     */

    int m_gmute_tracks;                 /* not included in .usr file    */

    /**
     *  The maximum number of patterns supported is given by the number of
     *  patterns supported in the panel (32) times the maximum number of
     *  sets (32), or 1024 patterns.
     */

    int m_max_sequence;

    /**
     *  The width of the main pattern/sequence grid, in pixels.  Affected by
     *  the m_mainwid_spacing value and m_window_scale.  Replaces c_mainwid_x.
     */

    int m_mainwid_x;

    /**
     *  The height of the main pattern/sequence grid, in pixels.
     *  Replaces c_mainwid_y.
     */

    int m_mainwid_y;

    /**
     *  The hardwired base width of the whole main window.  If m_window_scale
     *  is significantly different from 1.0, then the accessor will scale this
     *  value.
     */

    int m_mainwnd_x;

    /**
     *  The hardwired base height of the whole main window.  Llike
     *  m_mainwnd_x, this value is scaled by the accessor, however, only if
     *  less than 1.0; otherwise, the top buttons expand way too much.
     */

    int m_mainwnd_y;

    /*
     *  All constant (unchanging) values go here.  They are not saved or read.
     */

    /**
     *  Provides the minimum zoom value, currently a constant.  It's value is
     *  1.
     */

    const int mc_min_zoom;

    /**
     *  Provides the maximum zoom value, currently a constant.  It's value was
     *  32, but is now 512, to allow for better presentation of high PPQN
     *  valued sequences.
     */

    const int mc_max_zoom;

    /**
     *  Permanent storage for the baseline, default PPQN used by Seq24.
     *  This value is necessary in order to keep user-interface elements
     *  stable when different PPQNs are used.  It is set to DEFAULT_PPQN.
     */

    const int mc_baseline_ppqn;

    /*
     *  [user-options]
     */

    /**
     *  Provides a temporary variable that can be set from the command line to
     *  cause the "user" state to be saved into the "user" configuration file.
     *
     *  Normally, this state is not saved.  It is not saved because there is
     *  currently no user-interface for editing it, and because it can pick up
     *  some command-line options, and it is not right to have them written
     *  to the "user" configuration file.
     *
     *  (The "rc" configuration file is a different case, having historically
     *  always been saved, and having a number of command-line options, such
     *  as JACK settings that should generally be permanent on a given
     *  system.)
     *
     *  Anyway, this flag can be set by the --user-save option.  This setting
     *  is never saved.  But note that, if no "user" configuration file is
     *  found, it is then saved anyway.
     */

    bool m_save_user_config;

    /**
     *  Indicates if the application is running headless.  That is, from the
     *  seq66cli command-line application/daemon.  We can do things when
     *  headless without having to worry about signalling Qt.
     */

    bool m_app_is_headless;

    /**
     *  Indicates if the application should be daemonized.  All options that
     *  begin with "option_" are options specific to a particular version of
     *  Seq66.  We don't anticipate having a lot of such options,
     *  so there's no need for a separate class to handle them.  These options
     *  are flagged on the command-line by the strings "-o" or "--option".
     */

    bool m_user_option_daemonize;

    /**
     *  If true, this value means that "-o log=..." (where the "..." is an
     *  optional filename) was specified on the command line.
     */

    bool m_user_use_logfile;

    /**
     *  If not empty, this file will be set up as the destination for all
     *  logging done by the errprint(), infoprint(), warnprint(), and printf()
     *  functions.  In other words, stdout and stderr will go to a log
     *  file instead.  Unless a full path is provided, this filename will be a
     *  base filename, with the path given by rc().config_directory()
     *  prepended to it.  That path is normally ~/.config/seq66, but can
     *  be modified on the command line via the -H (--home) option.
     *
     *  This file can also be specified by the "-o log=filename" option.
     */

    std::string m_user_option_logfile;

    /*
     *  [user-work-arounds]
     */

    /*
     *  [user-ui-tweaks]
     */

    /**
     *  Defines the key height in the Kepler34 sequence editor.  Defaults to
     *  12 pixels (8 is actually a bit nicer IMHO).
     */

    int m_user_ui_key_height;

    /**
     *  Turns on the replacement of the Qt 5 qseqeditframe (now moved to
     *  contrib/code) with the larger and more functional qseqeditframe64, in
     *  the "Edit" tab.  A Kepler34 adaptation. Now permanently true.
     */

    bool m_user_ui_seqedit_in_tab;

    /**
     *  Provides the name of an optional Qt style-sheet, located in the active
     *  Seq66 configuration directory.  By default, this name is empty and not
     *  used.  It present, it contains the base name of the sheet (e.g.
     *  "qseq66.qss".  (It can contain a path, in order to support a universal
     *  style-sheet).
     */

    std::string m_user_ui_style_sheet;

    /**
     *  Indicates to resume notes that are "in progress" upon a sequence
     *  toggle. A Kepler34 adaptation.
     */

    bool m_resume_note_ons;

    /**
     *  The size of the fingerprint to use.  The default size is 32, but can
     *  be made larger, at the expense of slowing down drawing slightly.
     */

    int m_fingerprint_size;

    /**
     *  Lets the progress-box in the loop-buttons be tailored in size, or even
     *  not drawn at all.  The defaults are -1, which means use the internal
     *  defaults in qloopbutton.
     */

    double m_progress_box_width;
    double m_progress_box_height;

    /**
     *  [user-session]
     *
     *  This value indicates to create and use a Non Session Manager (or New
     *  Session Manager) client.  The name of the option in the "usr" file is
     *  "session", as in "session = nsm".
     */

    session m_session_manager;

    /**
     *  This optional value can be used to attach to an existing name session.
     *  This feature is mostly for trouble-shooting.  This option, "url", if
     *  set, provides a URL such as this sample for NSM:
     *  "osc.udp://mlsasus:15344".  To use it, one can change the normally
     *  pseudo-random port number to a set value:
     *
     *      nsmd --osc-port 15344
     *
     *  The daemon will emit the full URL to the console, and this value can
     *  be placed in qseq66.usr via the value "url = osc.udp://mlsasus:15344".
     */

    std::string m_session_url;

    /**
     *  Indicates if a session was able to be activated.  This item is not
     *  stored in the "usr" file.  It is treated like a pseudo-global flag.
     */

    bool m_in_session;

    /**
     *  [new-pattern-editor]
     *
     *  A new feature, in progress.
     */

    bool m_new_pattern_armed;
    bool m_new_pattern_thru;
    bool m_new_pattern_record;
    bool m_new_pattern_qrecord;
    recordstyle m_new_pattern_recordstyle;

public:

    usrsettings ();

    /*
     * Not using default at present.  Both could be modified by the normalize()
     * call.  However copying and assignment aren't even used at present.
     * Comment it out for now.
     */

#if defined USE_USRSETTINGS_COPYING
    usrsettings (const usrsettings & rhs);
    usrsettings & operator = (const usrsettings & rhs);
#else
    usrsettings (const usrsettings & rhs) = delete;
    usrsettings & operator = (const usrsettings & rhs) = delete;
#endif

    virtual void set_defaults () override;
    virtual void normalize () override;

    bool add_bus (const std::string & alias);
    bool add_instrument (const std::string & instname);
    void clear_buses_and_instruments ()
    {
        m_midi_buses.clear();
        m_instruments.clear();
    }

    /**
     * \getter
     *      Unlike the non-const version this function is public.
     *      Cannot append the const specifier.
     */

    const usermidibus & bus (int index) // const
    {
        return private_bus(index);
    }

    /**
     *  Unlike the non-const version this function is public.  Cannot append
     *  the const specifier.
     */

    const userinstrument & instrument (int index) // const
    {
        return private_instrument(index);
    }

    int bus_count () const
    {
        return int(m_midi_buses.size());
    }

    bool set_bus_instrument (int index, int channel, int instrum);

    int bus_instrument (int buss, int channel)
    {
        return bus(buss).instrument(channel);
    }

    const std::string & bus_name (int buss)
    {
        return bus(buss).name();
    }

    int instrument_count () const
    {
        return int(m_instruments.size());
    }

    bool set_instrument_controllers
    (
        int index, int cc, const std::string & ccname, bool isactive
    );

    /**
     * \getter m_instruments[instrument].instrument (name of instrument).
     */

    const std::string & instrument_name (int instrum)
    {
        return instrument(instrum).name();
    }

    /**
     *  Gets the correct instrument number from the buss and channel, and then
     *  looks up the name of the instrument.
     */

    const std::string & instrument_name (int buss, int channel)
    {
        int instrum = bus_instrument(buss, channel);
        return instrument(instrum).name();
    }

    bool instrument_controller_active (int instrum, int cc)
    {
        return instrument(instrum).controller_active(cc);
    }

    /**
     *  A convenience function so that the caller doesn't have to get the
     *  instrument number from the bus_instrument() member function.  It also
     *  has a shorter name.
     */

    bool controller_active (int buss, int channel, int cc)
    {
        int instrum = bus_instrument(buss, channel);
        return instrument(instrum).controller_active(cc);
    }

    const std::string & instrument_controller_name (int instrum, int cc)
    {
        return instrument(instrum).controller_name(cc);
    }

    /**
     * \getter m_instruments[instrument].controllers_active[controller].
     *  A convenience function so that the caller doesn't have to get the
     *  instrument number from the bus_instrument() member function.  It also
     *  has a shorter name.
     */

    const std::string & controller_name (int buss, int channel, int cc)
    {
        int instrum = bus_instrument(buss, channel);
        return instrument(instrum).controller_name(cc);
    }

public:

    float window_scale () const
    {
        return m_window_scale;
    }

    float window_scale_x () const
    {
        return m_window_scale;
    }

    float window_scale_y () const
    {
        return m_window_scale_y;
    }

    bool window_scale (float winscale, float winscaley = 0.0);
    bool parse_window_scale(const std::string & source);

    /**
     *  Returns true if we're increasing the size of the main window.
     *  In order to avoid double-precision issues, the limit is 1.01 rather
     *  than 1.0.
     */

    bool window_scaled_up () const
    {
        return m_window_scale >= 1.01f || m_window_scale_y >= 1.01f;
    }

    /**
     *  Returns true if we're reducing the size of the main window.
     *  In order to avoid double-precision issues, the limit is 0.99 rather
     *  than 1.0.
     */

    bool window_scaled_down () const
    {
        return m_window_scale <= 0.99f || m_window_scale_y <= 0.99f;
    }

    /**
     *  Returns true if the window is scaled.
     */

    bool window_is_scaled () const
    {
        return window_scaled_up() || window_scaled_down();
    }

    int scale_font_size (int value) const;

    int scale_size (int value) const
    {
        return int(m_window_scale * value + 0.5);
    }

    int scale_size_y (int value) const
    {
        return int(m_window_scale_y * value + 0.5);
    }

    int mainwnd_rows () const
    {
        return m_mainwnd_rows;
    }

    int mainwnd_cols () const
    {
        return m_mainwnd_cols;
    }

    int set_size () const
    {
        return m_mainwnd_rows * m_mainwnd_cols;
    }

    bool is_variset () const;
    bool is_default_mainwid_size () const;
    bool vertically_compressed () const;
    bool horizontally_compressed () const;

    int seqs_in_set () const
    {
        return m_seqs_in_set;
    }

    int gmute_tracks () const
    {
        return m_gmute_tracks;
    }

    int max_sequence () const
    {
        return m_max_sequence;
    }

    int total_seqs () const
    {
        return m_total_seqs;            /* not included in .usr file    */
    }

    /**
     * \getter m_seqchars_x, not user modifiable, not saved.
     */

    int seqchars_x () const
    {
        return m_seqchars_x;
    }

    /**
     * \getter m_seqchars_y, not user modifiable, not saved.
     */

    int seqchars_y () const
    {
        return m_seqchars_y;
    }

    int mainwid_spacing () const
    {
        return scale_size(m_mainwid_spacing);
    }

    int mainwnd_x () const;
    int mainwnd_y () const;

    int zoom () const
    {
        return m_current_zoom;
    }

    void zoom (int value);      /* seqedit can change this one */

    bool adapt_zoom () const
    {
        return m_current_zoom == SEQ66_USE_ZOOM_POWER_OF_2;
    }

    bool global_seq_feature () const
    {
        return m_global_seq_feature_save;
    }

    void global_seq_feature (bool flag)
    {
        m_global_seq_feature_save = flag;
    }

    int seqedit_scale () const
    {
        return m_seqedit_scale;
    }

    void seqedit_scale (int scale)
    {
        if (legal_scale(scale))
            m_seqedit_scale = scale;
    }

    int seqedit_key () const
    {
        return m_seqedit_key;
    }

    void seqedit_key (int key)
    {
        if (legal_key(key))
            m_seqedit_key = key;
    }

    int seqedit_bgsequence () const
    {
        return m_seqedit_bgsequence;
    }

    /**
     * \setter m_seqedit_bgsequence
     *
     *      Note that seq::legal() allows the seq::limit() (0x800 = 2048)
     *      value, to turn off the use of a background sequence.
     */

    void seqedit_bgsequence (int seqnum)
    {
        m_seqedit_bgsequence = seqnum;
    }

    bool progress_bar_thick () const
    {
        return m_progress_bar_thick;
    }

    bool inverse_colors () const
    {
        return m_inverse_colors;
    }

    int window_redraw_rate () const
    {
        return m_window_redraw_rate_ms;
    }

    bool save_user_config () const
    {
        return m_save_user_config;
    }

    void save_user_config (bool flag)
    {
        m_save_user_config = flag;
    }

protected:

    void mainwnd_rows (int value);
    void mainwnd_cols (int value);

    /*
     * This is a derived value, not settable by the user.  We will need to fix
     * this at some point; it is currently a usrfile option!
     */

    void seqchars_x (int value);
    void seqchars_y (int value);
    void mainwid_spacing (int value);

    /*
     *  These values are calculated from other values in the normalize()
     *  function:
     *
     *  void seqs_in_set (int value);
     *  void gmute_tracks (int value);
     *  void max_sequence (int value);
     *  void mainwid_x (int value);
     *  void mainwid_y (int value);
     */

    void dump_summary();

public:

    int default_ppqn () const
    {
        return m_default_ppqn;
    }

    int midi_ppqn () const
    {
        return m_midi_ppqn;     /* current PPQN, either default or file */
    }

    bool use_file_ppqn () const
    {
        return m_use_file_ppqn;
    }

    int file_ppqn () const
    {
        return m_file_ppqn;
    }

    void use_file_ppqn (bool flag)
    {
        m_use_file_ppqn = flag;
    }

    void file_ppqn (int p)
    {
        m_file_ppqn = p;
    }

    int midi_beats_per_bar () const
    {
        return m_midi_beats_per_measure;
    }

    midibpm midi_bpm_minimum () const
    {
        return m_midi_bpm_minimum;
    }

    midibpm midi_beats_per_minute () const
    {
        return m_midi_beats_per_minute;
    }

    midibpm midi_bpm_maximum () const
    {
        return m_midi_bpm_maximum;
    }

    long tap_bpm_timeout () const
    {
        return SEQ66_TAP_BUTTON_TIMEOUT;
    }

    int midi_beat_width () const
    {
        return m_midi_beat_width;
    }

    bussbyte midi_buss_override () const
    {
        return m_midi_buss_override;
    }

    int velocity_override () const
    {
        return m_velocity_override;
    }

    int bpm_precision () const
    {
        return m_bpm_precision;
    }

    midibpm bpm_step_increment () const
    {
        return m_bpm_step_increment;
    }

    midibpm bpm_page_increment () const
    {
        return m_bpm_page_increment;
    }

    int min_zoom () const
    {
        return mc_min_zoom;
    }

    int max_zoom () const
    {
        return mc_max_zoom;
    }

    int baseline_ppqn () const
    {
        return mc_baseline_ppqn;
    }

    bool app_is_headless () const
    {
        return m_app_is_headless;
    }

    bool option_daemonize () const
    {
        return m_user_option_daemonize;
    }

    bool option_use_logfile () const
    {
        return m_user_use_logfile;
    }

    std::string option_logfile () const;

    int key_height () const
    {
        return m_user_ui_key_height;
    }

    bool valid_key_height (int h) const
    {
        return h >= SEQ66_SEQKEY_HEIGHT_MIN && h <= SEQ66_SEQKEY_HEIGHT_MAX;
    }

    /*
     * Deprecated and hardwired to true.  We now use the new qseqeditframe64
     * all the time, and pare it down slightly when embedded in the Edit tab.
     */

    bool use_new_seqedit () const
    {
        return true;
    }

    const std::string & style_sheet () const
    {
        return m_user_ui_style_sheet;
    }

    bool resume_note_ons () const
    {
        return m_resume_note_ons;
    }

    int fingerprint_size () const
    {
        return m_fingerprint_size;
    }

    double progress_box_width () const
    {
        return m_progress_box_width;
    }

    double progress_box_height () const
    {
        return m_progress_box_height;
    }

    session session_manager () const
    {
        return m_session_manager;
    }

    std::string session_manager_name () const;

    bool wants_nsm_session () const
    {
        return m_session_manager == session::nsm;
    }

    bool wants_lash_session () const
    {
        return m_session_manager == session::lash;
    }

    bool in_session () const
    {
        return m_in_session;
    }

    const std::string & session_url () const
    {
        return m_session_url;
    }

    bool new_pattern_armed () const
    {
        return m_new_pattern_armed;
    }

    bool new_pattern_thru () const
    {
        return m_new_pattern_thru;
    }

    bool new_pattern_record () const
    {
        return m_new_pattern_record;
    }

    bool new_pattern_qrecord () const
    {
        return m_new_pattern_qrecord;
    }

    recordstyle new_pattern_recordstyle () const
    {
        return m_new_pattern_recordstyle;
    }

    int new_pattern_recordcode () const
    {
        return static_cast<int>(m_new_pattern_recordstyle);
    }

public:         // used in main application module and the usrfile class

    void progress_bar_thick (bool flag)
    {
        m_progress_bar_thick = flag;
    }

    void inverse_colors (bool flag)
    {
        m_inverse_colors = flag;
    }

    void window_redraw_rate (int ms);

    void app_is_headless (bool flag)
    {
        m_app_is_headless = flag;
    }

    void option_daemonize (bool flag)
    {
        m_user_option_daemonize = flag;
    }

    void option_use_logfile (bool flag)
    {
        m_user_use_logfile = flag;
    }

    void option_logfile (const std::string & logfile)
    {
        m_user_option_logfile = logfile;
    }

    void key_height (int h)
    {
        if (valid_key_height(h))
            m_user_ui_key_height = h;
    }

    void style_sheet (const std::string & s)
    {
        m_user_ui_style_sheet = s;
    }

    void resume_note_ons (bool f)
    {
        m_resume_note_ons = f;
    }

    void fingerprint_size (int sz)
    {
        if ((sz >= 32 && sz <= 128) || sz == 0)
            m_fingerprint_size = sz;
    }

    void progress_box_size (double w, double h);
    void session_manager (const std::string & sm);

    void in_session (bool f)
    {
        m_in_session = f;
    }

    void session_url (const std::string & value)
    {
        m_session_url = value;
    }

    void new_pattern_armed (bool flag)
    {
        m_new_pattern_armed = flag;
    }

    void new_pattern_thru (bool flag)
    {
        m_new_pattern_thru = flag;
    }

    void new_pattern_record (bool flag)
    {
        m_new_pattern_record = flag;
    }

    void new_pattern_qrecord (bool flag)
    {
        m_new_pattern_qrecord = flag;
    }

    void new_pattern_recordstyle (recordstyle style)
    {
        if (style < recordstyle::max)
            m_new_pattern_recordstyle = style;
    }

    void default_ppqn (int ppqn);
    void midi_ppqn (int ppqn);
    void midi_buss_override (bussbyte buss);
    void velocity_override (int vel);
    void bpm_precision (int precision);
    void bpm_step_increment (midibpm increment);
    void bpm_page_increment (midibpm increment);

protected:

    void midi_beats_per_bar (int beatsperbar);
    void midi_bpm_minimum (midibpm beatsperminute);
    void midi_beats_per_minute (midibpm beatsperminute);
    void midi_bpm_maximum (midibpm beatsperminute);
    void midi_beat_width (int beatwidth);

private:

    usermidibus & private_bus (int buss);
    userinstrument & private_instrument (int instrum);

};          // class usrsettings

}           // namespace seq66

#endif      // SEQ66_USRSETTINGS_HPP

/*
 * usrsettings.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

