#include "sqlitestudio.h"
#include <QApplication>

#include <QDebug>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
//    // 启用CRT内存泄漏检测
    #ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);	// 警告信息输出到调试器的输出窗口
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);	// 输出到控制台
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);	// 不可恢复的错误信息，显示中断窗口
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);	// 断言为false，显示中断窗口
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	_CrtSetBreakAlloc(475);
	_CrtSetBreakAlloc(285);
	_CrtSetBreakAlloc(286);
	#endif

	SQLiteStudio w;
	w.show();

    return a.exec();
}
