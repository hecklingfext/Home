--[[
Conky Widgets by londonali1010 (2009)

This script is meant to be a "shell" to hold a suite of widgets for use in Conky.

To configure:
+ Copy the widget's code block (will be framed by --(( WIDGET NAME )) and --(( END WIDGET NAME )), with "[" instead of "(") somewhere between "require 'cairo'" and "function conky_widgets()", ensuring not to paste into another widget's code block
+ To call the widget, add the following just before the last "end" of the entire script:
	cr = cairo_create(cs)
	NAME_OF_FUNCTION(cr, OPTIONS)
	cairo_destroy(cr)
+ Replace OPTIONS with the options for your widget (should be specified in the widget's code block) 

Call this script in Conky using the following before TEXT (assuming you save this script to ~/scripts/conky_widgets.lua):
	lua_load ~/scripts/conky_widgets.lua
	lua_draw_hook_pre widgets
	
Changelog:
+ v1.0 -- Original release (17.10.2009)
]]

require 'cairo'
require 'imlib2'

--[[ AIR CLOCK WIDGET ]]
--[[ Options (xc, yc, size):
	"xc" and "yc" are the x and y coordinates of the centre of the clock, in pixels, relative to the top left of the Conky window
	"size" is the total size of the widget, in pixels ]]

function air_clock(cr, xc, yc, size)
	local offset = 0 
	
	shadow_width = size * 0.03
	shadow_xoffset = 0
	shadow_yoffset = size * 0.01
	
	if shadow_xoffset >= shadow_yoffset then
		offset = shadow_xoffset
	else offset = shadow_yoffset
	end
	
	local clock_r = (size - 2 * offset) / (2 * 1.25)
		
	show_seconds=true
	
	-- Grab time
	
	local hours=os.date("%I")
	local mins=os.date("%M")
	local secs=os.date("%S")
	
	secs_arc=(2*math.pi/60)*secs
	mins_arc=(2*math.pi/60)*mins
	hours_arc=(2*math.pi/12)*hours+mins_arc/12
	
	-- Draw hour hand
	
	xh=xc+0.7*clock_r*math.sin(hours_arc)
	yh=yc-0.7*clock_r*math.cos(hours_arc)
	cairo_move_to(cr,xc,yc)
	cairo_line_to(cr,xh,yh)
	
	cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND)
	cairo_set_line_width(cr,5)
	cairo_set_source_rgba(cr,0,0,0,0.75)
	cairo_stroke(cr)
	
	-- Draw minute hand
	
	xm=xc+0.9*clock_r*math.sin(mins_arc)
	ym=yc-0.9*clock_r*math.cos(mins_arc)
	cairo_move_to(cr,xc,yc)
	cairo_line_to(cr,xm,ym)
	
	cairo_set_line_width(cr,3)
	cairo_stroke(cr)
	
	-- Draw seconds hand
	
	if show_seconds then
		xs=xc+0.9*clock_r*math.sin(secs_arc)
		ys=yc-0.9*clock_r*math.cos(secs_arc)
		cairo_move_to(cr,xc,yc)
		cairo_line_to(cr,xs,ys)
	
		cairo_set_line_width(cr,1)
		cairo_stroke(cr)
	end
end

--[[ END AIR CLOCK WIDGET ]]

--[[ RING WIDGET ]]
--[[ Options (name, arg, max, bg_colour, bg_alpha, xc, yc, radius, thickness, start_angle, end_angle):
	"name" is the type of stat to display; you can choose from 'cpu', 'memperc', 'fs_used_perc', 'battery_used_perc'.
	"arg" is the argument to the stat type, e.g. if in Conky you would write ${cpu cpu0}, 'cpu0' would be the argument. If you would not use an argument in the Conky variable, use ''.
	"max" is the maximum value of the ring. If the Conky variable outputs a percentage, use 100.
	"bg_colour" is the colour of the base ring.
	"bg_alpha" is the alpha value of the base ring.
	"fg_colour" is the colour of the indicator part of the ring.
	"fg_alpha" is the alpha value of the indicator part of the ring.
	"x" and "y" are the x and y coordinates of the centre of the ring, relative to the top left corner of the Conky window.
	"radius" is the radius of the ring.
	"thickness" is the thickness of the ring, centred around the radius.
	"start_angle" is the starting angle of the ring, in degrees, clockwise from top. Value can be either positive or negative.
	"end_angle" is the ending angle of the ring, in degrees, clockwise from top. Value can be either positive or negative, but must be larger (e.g. more clockwise) than start_angle. ]]

function ring(cr, name, arg, max, bgc, bga, fgc, fga, xc, yc, r, t, sa, ea)
	local function rgb_to_r_g_b(colour,alpha)
		return ((colour / 0x10000) % 0x100) / 255., ((colour / 0x100) % 0x100) / 255., (colour % 0x100) / 255., alpha
	end
	
	local function draw_ring(pct)
		local angle_0=sa*(2*math.pi/360)-math.pi/2
		local angle_f=ea*(2*math.pi/360)-math.pi/2
		local pct_arc=pct*(angle_f-angle_0)

		-- Draw background ring

		cairo_arc(cr,xc,yc,r,angle_0,angle_f)
		cairo_set_source_rgba(cr,rgb_to_r_g_b(bgc,bga))
		cairo_set_line_width(cr,t)
		cairo_stroke(cr)
	
		-- Draw indicator ring

		cairo_arc(cr,xc,yc,r,angle_0,angle_0+pct_arc)
		cairo_set_source_rgba(cr,rgb_to_r_g_b(fgc,fga))
		cairo_stroke(cr)
	end
	
	local function setup_ring()
		local str = ''
		local value = 0
		
		str = string.format('${%s %s}', name, arg)
		str = conky_parse(str)
		
		value = tonumber(str)
		if value == nil then value = 0 end
		pct = value/max
		
		draw_ring(pct)
	end	
	
	local updates=conky_parse('${updates}')
	update_num=tonumber(updates)
	
	if update_num>5 then setup_ring() end
end

--[[ END RING WIDGET ]]

--[[ RING (COUNTER-CLOCKWISE) WIDGET ]]
--[[ v1.1 by londonali1010 (2009) ]]
--[[ Options (name, arg, max, bg_colour, bg_alpha, xc, yc, radius, thickness, start_angle, end_angle):
	"name" is the type of stat to display; you can choose from 'cpu', 'memperc', 'fs_used_perc', 'battery_used_perc'.
	"arg" is the argument to the stat type, e.g. if in Conky you would write ${cpu cpu0}, 'cpu0' would be the argument. If you would not use an argument in the Conky variable, use ''.
	"max" is the maximum value of the ring. If the Conky variable outputs a percentage, use 100.
	"bg_colour" is the colour of the base ring.
	"bg_alpha" is the alpha value of the base ring.
	"fg_colour" is the colour of the indicator part of the ring.
	"fg_alpha" is the alpha value of the indicator part of the ring.
	"x" and "y" are the x and y coordinates of the centre of the ring, relative to the top left corner of the Conky window.
	"radius" is the radius of the ring.
	"thickness" is the thickness of the ring, centred around the radius.
	"start_angle" is the starting angle of the ring, in degrees, counter-clockwise from top. Value can be either positive or negative.
	"end_angle" is the ending angle of the ring, in degrees, counter-clockwise from top. Value can be either positive or negative, but must be larger (e.g. more counter-clockwise) than start_angle. ]]

function ring_ccw(cr, name, arg, max, bgc, bga, fgc, fga, xc, yc, r, t, sa, ea)
	local function rgb_to_r_g_b(colour, alpha)
		return ((colour / 0x10000) % 0x100) / 255., ((colour / 0x100) % 0x100) / 255., (colour % 0x100) / 255., alpha
	end
	
	local function draw_ring(pct)
		local angle_0 = sa * (2 * math.pi/360) - math.pi/2
		local angle_f = ea * (2 * math.pi/360) - math.pi/2
		local pct_arc = pct * (angle_0 - angle_f)

		-- Draw background ring

		cairo_arc_negative(cr, xc, yc, r, angle_0, angle_f)
		cairo_set_source_rgba(cr, rgb_to_r_g_b(bgc, bga))
		cairo_set_line_width(cr, t) 
		cairo_stroke(cr)
	
		-- Draw indicator ring

		cairo_arc_negative(cr, xc, yc, r, angle_0, angle_0 - pct_arc)
		cairo_set_source_rgba(cr, rgb_to_r_g_b(fgc, fga))
		cairo_stroke(cr)
	end
	
	local function setup_ring()
		local str = ''
		local value = 0
		
		str = string.format('${%s %s}', name, arg)
		str = conky_parse(str)
		
		value = tonumber(str)
		if value == nil then value = 0 end
		pct = value/max
		
		draw_ring(pct)
	end	
	
	local updates = conky_parse('${updates}')
	update_num = tonumber(updates)
	
	if update_num > 5 then setup_ring() end
end

--[[ END RING (COUNTER-CLOCKWISE) WIDGET ]]

--[[ PHOTO ALBUM WIDGET ]]
--[[ by londonali1010 (2009) ]]
--[[ Options (album_dir, xc, yc, w_max, h_max, t, update_interval)
    "album_dir" is the directory containing the images for your photo album; please note that the path must be absolute (e.g. no "~")
    "xc" and "yc" are the coordinates of the centre of the photo album, relative to the top left corner of the Conky window, in pixels
    "w_max" and "h_max" are the maximum dimensions, in pixels, that you want the widget to be.  The script will ensure that the photo album fits inside the box bounded by w_max and h_max
    "t" is the thickness of the frame, in pixels
    "update_interval" is the number of Conky updates between refreshes ]]

function photo_album(album_dir, xc, yc, w_max, h_max, t, update_interval)
    local function get_file_to_use()
        num_files = tonumber(conky_parse("${exec ls -A " .. album_dir .. " | wc -l}"))
        if num_files == nil then num_files = 0 end
        if num_files == 0 then return "none" end
    
        updates = tonumber(conky_parse("${updates}"))
        whole = math.ceil(updates/update_interval)
    
        if whole <= num_files
        then
            num_file_to_show = whole
        else
            whole = whole % num_files
            num_file_to_show = whole
        end
    
        return conky_parse("${exec ls " .. album_dir .. " | sed -n " .. num_file_to_show .. "p}")
    end

    function init_drawing_surface()
        imlib_set_cache_size(4096 * 1024)
        imlib_context_set_dither(1)
    end

    function draw_frame()
        cairo_rectangle(cr, xc - width/2 - t, yc - height/2 - t, width + 2*t, height + 2*t)
        cairo_set_source_rgba(cr, 1, 1, 1, 0.8)
        cairo_fill(cr)   
    end

    function draw_image()
        init_drawing_surface()
    
        image = imlib_load_image("/home/awross/Pictures/stock/lightning.png")
        if image == nil then return end
        imlib_context_set_image(image)
	
    	w_img, h_img = imlib_image_get_width(), imlib_image_get_height()
    	if w_img >= h_img
    	then
    	    width = w_max - 2*t
    	    height = width * (h_img/w_img)
    	else
    	    height = h_max - 2*t
    	    width = height * (w_img/h_img)
    	end
    	
    	draw_frame()
    	
    	buffer = imlib_create_image(width, height)
    	imlib_context_set_image(buffer)
    	
    	imlib_blend_image_onto_image(image, 0, 0.5, 0, w_img, h_img, 0, 0, width, height)
    	imlib_context_set_image(image)
    	imlib_free_image()
    	
    	imlib_context_set_image(buffer)
    	imlib_render_image_on_drawable(xc - width/2, yc - height/2)
    	imlib_free_image()
    end

    if conky_window == nil then return end
    filename = get_file_to_use()
    if filename == "none"
    then
        print(album_dir .. ": No files found")
    else
        draw_image()
    end
end

--[[ END PHOTO ALBUM WIDGET ]]

function conky_widgets()
	if conky_window == nil then return end
	local cs = cairo_xlib_surface_create(conky_window.display, conky_window.drawable, conky_window.visual, conky_window.width, conky_window.height)

-- 	cr = cairo_create(cs)
--	photo_album("/home/awross/Pictures/wallpapers/", 500, 600, 192*5, 160*5, 10, 2)
--	cairo_destroy(cr)

	cr = cairo_create(cs)
	air_clock(cr, 515, 615, 700) -- options: xc, yc, size
	cairo_destroy(cr)

-- Middle rings
	-- tl	
	cr = cairo_create(cs) 
	ring(cr, 'fs_used_perc', '/', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 570, 636, 450, 10, -89.9, -5)
	cairo_destroy(cr)
	-- bl
	cr = cairo_create(cs) 
	ring_ccw(cr, 'fs_used_perc', '/home', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 570, 636, 450, 10, -90.1, -175)
	cairo_destroy(cr)
	-- tr
	cr = cairo_create(cs) 
	ring_ccw(cr, 'fs_used_perc', '/video', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 450, 636, 450, 10, 89.9, 5)
	cairo_destroy(cr)
	-- br
	cr = cairo_create(cs) 
	ring(cr, 'fs_used_perc', '/data', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 450, 636, 450, 10, 90.1, 175) 
	cairo_destroy(cr)

-- Top rings
	-- right
	cr = cairo_create(cs)
	ring(cr, 'cpu', 'CPU1', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 520, 63, 125, 10, 91, 175) 
	cairo_destroy(cr)

-- 	cr = cairo_create(cs)
-- 	ring_ccw(cr, 'fs_used_perc', '/data', 100, 0xFFFFFF, 0.5, 0x000000, 1.0, 468, 273, 115, 10, 89, 20) 
-- 	cairo_destroy(cr)
	-- left
	cr = cairo_create(cs)
	ring_ccw(cr, 'cpu', 'CPU0', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 502, 63, 125, 10, -91, -174)
	cairo_destroy(cr)

-- 	cr = cairo_create(cs)
-- 	ring(cr, 'fs_used_perc', '/home', 100, 0xFFFFFF, 0.5, 0x000000, 1.0, 551, 273, 115, 10, -89, -20) 
-- 	cairo_destroy(cr)

-- Bottom rings
	-- right
	cr = cairo_create(cs)
	ring_ccw(cr, 'cpu', 'CPU3', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 520, 1209, 125, 10, 89, 5) 
	cairo_destroy(cr)

-- 	cr = cairo_create(cs)
-- 	ring_ccw(cr, 'fs_used_perc', '/data', 100, 0xFFFFFF, 0.5, 0x000000, 1.0, 468, 273, 115, 10, 89, 20) 
-- 	cairo_destroy(cr)

	-- left
	cr = cairo_create(cs)
	ring(cr, 'cpu', 'CPU2', 100, 0x878787, 1.0, 0xFFFFFF, 1.0, 500, 1209, 125, 10, -89, -5) 
	cairo_destroy(cr)
end
