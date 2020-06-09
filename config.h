/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 2;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = {"JetBrains Mono:style=Medium:pixelsize=16:antialias=true:autohint=true"};
static const char dmenufont[]       = {"JetBrains Mono:style=Medium:pixelsize=16:antialias=true:autohint=true"};
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#0d0d0d";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };

// WM_CLASS Picker:
// xprop | grep WM_CLASS | awk '{print $4}'
//
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",            NULL,       NULL,       0,            1,           -1 },
//  { "firefox",         NULL,       NULL,       1 << 1,       0,           -1 },
	{ "st",              NULL,       NULL,       0     ,       0,           -1 },
	{ "Slack",           NULL,       NULL,       1 << 3,       0,           -1 },
	{ "Spotify",         NULL,       NULL,       1 << 6,       0,           -1 },
	{ "rdesktop",        NULL,       NULL,       1 << 8,       0,           -1 },
	{ "qBittorrent",     NULL,       NULL,       1 << 7,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "",      tile },    /* first entry is default */
	{ "",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-c", "-l", "20", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
/*************************** CUSTOM ****************************************/
static const char *firefox[]  = { "firefox", NULL };
static const char *clipmenu[]  = { "clipmenu", NULL };
static const char *pulsemixer[]  = { "st", "pulsemixer", NULL};
static const char *vifm[]  = { "st", "vifm", NULL};
static const char *rhd[] = { "rhd", NULL };
static const char *rhdfhd[] = { "rhd-fullhd-mon", NULL };
static const char *rhd2mon[] = { "rhd-2mon", NULL };
static const char *dmenueditconfigs[] = { "dmenu-edit-configs", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "145x35", NULL };
static const char *scratchcalcmd[] = { "st", "-t", scratchpadname, "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
static const char *screenshot[] = { "flameshot", "gui", "-p", "/tmp", NULL };
static const char *taskellmenu[] = { "st", "taskell-menu", NULL };
// static const char *trans[] = {"echo -e '' | dmenu -p 'Translate:' | trans -v", NULL };
/*************************** CUSTOM - Media Keys ****************************************/
static const char *brupcmd[]    = { "xbacklight", "-inc", "5", NULL };
static const char *brdowncmd[]  = { "xbacklight", "-dec", "5", NULL };
static const char *mutecmd[]    = { "pactl",      "set-sink-mute", "0", "toggle", NULL };
static const char *miccmd[]     = { "pactl",      "set-source-mute", "1", "toggle", NULL };
static const char *volupcmd[]   = { "pactl",      "set-sink-volume", "0", "+5%", NULL };
static const char *voldowncmd[] = { "pactl",      "set-sink-volume", "0", "-5%", NULL };

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static const char *statuscmds[] = { "notify-send Mouse$BUTTON" };
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

#include "movestack.c"
static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_grave,  spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY|ShiftMask,		        XK_Return, togglescratch,  {.v = scratchpadcmd } },
    { MODKEY,		                XK_equal,  togglescratch,  {.v = scratchcalcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
    /*************************** CUSTOM ****************************************/
	{ MODKEY|ShiftMask,             XK_f,      spawn,          {.v = firefox } },
	{ MODKEY,                       XK_w,      spawn,          {.v = clipmenu } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = pulsemixer } },
	{ Mod4Mask,                     XK_e,      spawn,          {.v = vifm } },
	{ 0,                            XK_Print,  spawn,          {.v = screenshot } },
	{ MODKEY|ControlMask,           XK_k,      spawn,          {.v = taskellmenu } },
	// { MODKEY|ControlMask,           XK_t,      spawn,          {.v = trans } },
    { Mod4Mask,                     XK_F1     ,spawn,          {.v = rhd} },
    { Mod4Mask,                     XK_F2     ,spawn,          {.v = rhdfhd} },
    { Mod4Mask,                     XK_F3     ,spawn,          {.v = rhd2mon} },
    { MODKEY|ShiftMask,             XK_e      ,spawn,          {.v = dmenueditconfigs} },
/*************************** CUSTOM - Media Keys ****************************************/
    { 0,                            XF86XK_AudioMute,         spawn, {.v = mutecmd } },
    { 0,                            XF86XK_AudioMicMute,      spawn, {.v = miccmd } },
    { 0,                            XF86XK_AudioLowerVolume,  spawn, {.v = voldowncmd } },
    { 0,                            XF86XK_AudioRaiseVolume,  spawn, {.v = volupcmd } },
    { 0,                            XF86XK_MonBrightnessUp,   spawn, {.v = brupcmd} },
    { 0,                            XF86XK_MonBrightnessDown, spawn, {.v = brdowncmd} },
/*************************** CUSTOM - COMMANDS ***************************************/
    { MODKEY|ShiftMask,		        XK_x,                     spawn, SHCMD("[ \"$(printf \"No\\nYes\" | dmenu -i -nb darkred -sb red -sf white -nf gray -p \"Shutdown computer?\")\" = Yes ] && shutdown -h now") },
   	{ MODKEY|ShiftMask,     		XK_r,       	          spawn, SHCMD("[ \"$(printf \"No\\nYes\" | dmenu -i -nb darkblue -sb blue -sf white -nf gray -p \"Reboot computer?\")\" = Yes ] && sudo reboot") },
	{ MODKEY|ShiftMask,             XK_z,	                  spawn, SHCMD("[ \"$(printf \"No\\nYes\" | dmenu -i -nb darkgreen -sb green -sf black -nf gray -p \"Hibernate computer?\")\" = Yes ] && systemctl suspend") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

