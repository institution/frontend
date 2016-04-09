#pragma once
#include <SDL2/SDL.h>

namespace frontend {
	
	using Event = SDL_Event;
	using EventType = SDL_EventType;

	EventType const
		//EventAudioDeviceAdded = SDL_AUDIODEVICEADDED,
		//EventAudioDeviceRemoved = SDL_AUDIODEVICEREMOVED,
		EventControllerAxisMotion = SDL_CONTROLLERAXISMOTION,
		EventControllerButtonDown = SDL_CONTROLLERBUTTONDOWN,
		EventControllerButtonUp = SDL_CONTROLLERBUTTONUP,
		EventControllerDeviceAdded = SDL_CONTROLLERDEVICEADDED,
		EventControllerDeviceRemoved = SDL_CONTROLLERDEVICEREMOVED,
		EventControllerDeviceMapped = SDL_CONTROLLERDEVICEREMAPPED,
		EventDollarGesture = SDL_DOLLARGESTURE,
		EventDollarRecord = SDL_DOLLARRECORD,
		EventDropFile = SDL_DROPFILE,
		//EventFingerMotion = SDL_FINGERMOTION,
		//EventFingerDown = SDL_FINGERDOWN
		EventFingerUp = SDL_FINGERUP,
		EventKeyDown = SDL_KEYDOWN,
		EventKeyUp = SDL_KEYUP,
		EventJoyAxisMotion = SDL_JOYAXISMOTION,
		EventJoyBallMotion = SDL_JOYBALLMOTION,
		EventJoyHatMotion = SDL_JOYHATMOTION,
		EventJoyButtonDown = SDL_JOYBUTTONDOWN,
		EventJoyButtonUp = SDL_JOYBUTTONUP,
		EventJoyDeviceAdded = SDL_JOYDEVICEADDED,
		EventJoyDeviceRemoved = SDL_JOYDEVICEREMOVED,
		EventMouseMotion = SDL_MOUSEMOTION,
		EventMouseButtonDown = SDL_MOUSEBUTTONDOWN,
		EventMouseButtonUp = SDL_MOUSEBUTTONUP,
		EventMouseWheel = SDL_MOUSEWHEEL,
		EventMultiGesture = SDL_MULTIGESTURE,
		EventQuit = SDL_QUIT,
		EventSysWMEvent = SDL_SYSWMEVENT,
		EventTextEditing = SDL_TEXTEDITING,
		EventTextInput = SDL_TEXTINPUT,
		EventUserEvent = SDL_USEREVENT,
		EventWindowEvent = SDL_WINDOWEVENT;

	void print_window_event(Event const& event);
	
}
