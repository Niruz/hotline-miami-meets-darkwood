
static void
PeekMessages()
{
	MSG msg;
	while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			Game::running = false;
		}
		else {

			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
}