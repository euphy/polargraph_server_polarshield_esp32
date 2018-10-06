


# Quick idempotent commands take a fixed time, but can happen anywhere
CMD_SETPOSITION = "C09,{distance_a},{distance_b},END"
CMD_PENDOWN_TO_PRESET = "C13,END"
CMD_PENDOWN_SET_HEIGHT = "C13,{new_down_position},END"
CMD_PENUP_TO_PRESET = "C14,END"
CMD_PENUP_SET_HEIGHT = "C14,{new_up_position},END"

# Slow simple commands that work even if they aren't on the machine
CMD_CHANGELENGTH = "C01,{target_a},{target_b},END"
CMD_DRAWPIXEL = "C05,{},{},END"
CMD_DRAWSCRIBBLEPIXEL = "C06,{},{},END"
CMD_CHANGEDRAWINGDIRECTION = "C08,{},{},END"
CMD_TESTPATTERN = "C10,{},{},END"
CMD_TESTPENWIDTHSQUARE = "C11,{},{},END"
CMD_PIXELDIAGNOSTIC = "C46,{},{},END"
CMD_AUTO_CALIBRATE = "C48,{},{},END"

# Slow contextual commands need to be in the drawable area
CMD_CHANGELENGTHDIRECT = "C17,{},{},END"
CMD_TESTPENWIDTHSCRIBBLE = "C12,{},{},END"
CMD_DRAWSAWPIXEL = "C15,,{},{},END"
CMD_DRAWCIRCLEPIXEL = "C16,{},{},END"
CMD_DRAWDIRECTIONTEST = "C28,{},{},END"
CMD_RANDOM_DRAW = "C36,{},{},END"
CMD_DRAW_SPRITE = "C39,{},{},END"
CMD_CHANGELENGTH_RELATIVE = "C40,{},{},END"
CMD_SWIRLING = "C41,{},{},END"
CMD_DRAW_RANDOM_SPRITE = "C42,{},{},END"
CMD_DRAW_NORWEGIAN = "C43,{},{},END"
CMD_DRAW_NORWEGIAN_OUTLINE = "C44,{},{},END"