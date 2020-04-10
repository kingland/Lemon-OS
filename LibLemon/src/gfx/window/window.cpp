#include <stdint.h>

#include <lemon/types.h>
#include <gfx/window/window.h>

//#include <runtime.h>

struct Surface;

handle_t _CreateWindow(win_info_t* wininfo){
	handle_t h;
	syscall(SYS_CREATE_WINDOW,(uintptr_t)wininfo,0,0,0,0);

	h = wininfo->handle;

	return h;
}

void _DestroyWindow(handle_t window){
	syscall(SYS_DESTROY_WINDOW, (uintptr_t)window, 0, 0, 0, 0);
}

void _PaintWindow(handle_t window, surface_t* surface){
	syscall(SYS_UPDATE_WINDOW, (uintptr_t)window, (uintptr_t)surface, 0, 0, 0);
}

Window* CreateWindow(win_info_t* info){
	handle_t handle = _CreateWindow(info);

	Window* win = new Window();
	win->handle = handle;
	memcpy(&win->info,info,sizeof(win_info_t));

	surface_t surface;
	surface.width = info->width;
	surface.height = info->height;

	surface.buffer = (uint8_t*)malloc((surface.width * 4) * info->height);

	surface.linePadding = 0;

	win->surface = surface;

	return win;
}

void DestroyWindow(Window* win){
	_DestroyWindow(win->handle);
	delete win;
}

void PaintWindow(Window* win){
	//if(!(win->info.flags & WINDOW_FLAGS_NOBACKGROUND))
		DrawRect(0,0,win->info.width, win->info.height, win->background, &win->surface);

	for(int i = 0; i < win->widgets.get_length(); i++){
		win->widgets.get_at(i)->Paint(&win->surface);
	}

	if(win->OnPaint){
		win->OnPaint(&win->surface);
	}

	_PaintWindow(win->handle, &win->surface);
}

void HandleMouseDown(Window* win, vector2i_t mousePos){
	for(int i = 0; i < win->widgets.get_length(); i++){
		rect_t widgetBounds = win->widgets.get_at(i)->bounds;
		if(widgetBounds.pos.x <= mousePos.x && widgetBounds.pos.y < mousePos.y && widgetBounds.pos.x + widgetBounds.size.x > mousePos.x && widgetBounds.pos.y + widgetBounds.size.y > mousePos.y){
			win->widgets.get_at(i)->OnMouseDown(mousePos);
			win->lastPressedWidget = i;
			break;
		}
	}
}

Widget* HandleMouseUp(Window* win, vector2i_t mousePos){
	if(win->lastPressedWidget >= 0){
		win->widgets[win->lastPressedWidget]->OnMouseUp(mousePos);
		return win->widgets[win->lastPressedWidget];
	}
	win->lastPressedWidget = -1;
	return NULL;
}

void AddWidget(Widget* widget, Window* win){
	win->widgets.add_back(widget);
}