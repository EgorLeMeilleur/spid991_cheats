#include "gui.h"
#include "hacks.h"
#include "offsets.h"

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
	Memory memory{ "csgo.exe" };

	globals::clientAdress = memory.GetModuleAddress("client.dll");
	globals::engineAdress = memory.GetModuleAddress("engine.dll");

	std::thread(hacks::VisualsThread, memory).detach();
	std::thread(hacks::PlayerThread, memory).detach();

	gui::CreateHWindow("BEST CHEATS", "BEST CHEATS CLASS");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::isRunning)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}