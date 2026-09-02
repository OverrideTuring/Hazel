#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
	Hazel::Log::Init();
	HZ_CORE_INFO("Hazel Engine starts.");
	HZ_CORE_WARN("Hazel Engine starts.");
	HZ_CORE_ERROR("Hazel Engine starts.");
	HZ_CORE_CRITICAL("Hazel Engine starts.");

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif