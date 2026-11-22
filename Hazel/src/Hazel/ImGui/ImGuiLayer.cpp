#include "hzpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include "GLFW/glfw3.h"

#include "Hazel/Application.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include <cfloat>
#include <cstring>

namespace Hazel {

	static ImGuiKey ToImGuiKey(int key)
	{
		// GLFW reports physical keys. Use the active layout for printable shortcuts.
		if (key >= GLFW_KEY_APOSTROPHE && key <= GLFW_KEY_WORLD_2) {
			const char* name = glfwGetKeyName(key, 0);
			if (name && name[0] != '\0' && name[1] == '\0') {
				const char character = name[0];
				if (character >= 'a' && character <= 'z')
					key = GLFW_KEY_A + character - 'a';
				else if (std::strchr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'`-=[]\\,;./", character))
					key = character;
			}
		}

		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
			return static_cast<ImGuiKey>(ImGuiKey_0 + key - GLFW_KEY_0);
		if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
			return static_cast<ImGuiKey>(ImGuiKey_A + key - GLFW_KEY_A);
		if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12)
			return static_cast<ImGuiKey>(ImGuiKey_F1 + key - GLFW_KEY_F1);
		if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9)
			return static_cast<ImGuiKey>(ImGuiKey_Keypad0 + key - GLFW_KEY_KP_0);

		switch (key) {
			case GLFW_KEY_TAB: return ImGuiKey_Tab;
			case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
			case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
			case GLFW_KEY_UP: return ImGuiKey_UpArrow;
			case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
			case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
			case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
			case GLFW_KEY_HOME: return ImGuiKey_Home;
			case GLFW_KEY_END: return ImGuiKey_End;
			case GLFW_KEY_INSERT: return ImGuiKey_Insert;
			case GLFW_KEY_DELETE: return ImGuiKey_Delete;
			case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
			case GLFW_KEY_SPACE: return ImGuiKey_Space;
			case GLFW_KEY_ENTER: return ImGuiKey_Enter;
			case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
			case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
			case GLFW_KEY_COMMA: return ImGuiKey_Comma;
			case GLFW_KEY_MINUS: return ImGuiKey_Minus;
			case GLFW_KEY_PERIOD: return ImGuiKey_Period;
			case GLFW_KEY_SLASH: return ImGuiKey_Slash;
			case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
			case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
			case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
			case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
			case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
			case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
			case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
			case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
			case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
			case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
			case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
			case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
			case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
			case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
			case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
			case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
			case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
			case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
			case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
			case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
			case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
			case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
			case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
			case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
			case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
			case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
			case GLFW_KEY_MENU: return ImGuiKey_Menu;
			default: return ImGuiKey_None;
		}
	}

	static void UpdateKeyModifiers(ImGuiIO& io, int modifiers)
	{
		io.AddKeyEvent(ImGuiMod_Ctrl, (modifiers & GLFW_MOD_CONTROL) != 0);
		io.AddKeyEvent(ImGuiMod_Shift, (modifiers & GLFW_MOD_SHIFT) != 0);
		io.AddKeyEvent(ImGuiMod_Alt, (modifiers & GLFW_MOD_ALT) != 0);
		io.AddKeyEvent(ImGuiMod_Super, (modifiers & GLFW_MOD_SUPER) != 0);
	}

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		const EventType eventType = event.GetEventType();
		ImGuiIO& io = ImGui::GetIO();
		switch (eventType) {
		case EventType::MouseMoved: {
			const auto& mouse = static_cast<const MouseMovedEvent&>(event);
			io.AddMousePosEvent(mouse.GetX(), mouse.GetY());
			break;
		}
		case EventType::MouseLeftWindow:
			io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
			break;
		case EventType::MouseButtonPressed:
		case EventType::MouseButtonReleased: {
			const auto& mouse = static_cast<const MouseButtonEvent&>(event);
			UpdateKeyModifiers(io, mouse.GetModifiers());
			const int button = mouse.GetMouseButton();
			if (button >= 0 && button < ImGuiMouseButton_COUNT)
				io.AddMouseButtonEvent(button, eventType == EventType::MouseButtonPressed);
			break;
		}
		case EventType::MouseScrolled: {
			const auto& scroll = static_cast<const MouseScrolledEvent&>(event);
			io.AddMouseWheelEvent(scroll.GetXOffset(), scroll.GetYOffset());
			break;
		}
		case EventType::KeyPressed:
			// ImGui derives key repeat from how long the key stays down.
			if (static_cast<const KeyPressedEvent&>(event).GetRepeatCount() != 0)
				break;
			[[fallthrough]];
		case EventType::KeyReleased: {
			const auto& key = static_cast<const KeyEvent&>(event);
			UpdateKeyModifiers(io, key.GetModifiers());
			io.AddKeyEvent(ToImGuiKey(key.GetKeyCode()), eventType == EventType::KeyPressed);
			break;
		}
		case EventType::KeyTyped:
			io.AddInputCharacter(static_cast<const KeyTypedEvent&>(event).GetCodepoint());
			break;
		case EventType::WindowFocus:
		case EventType::WindowLostFocus:
			io.AddFocusEvent(eventType == EventType::WindowFocus);
			break;
		default:
			break;
		}
	}
}
