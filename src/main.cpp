#include <QApplication>
#include <QCommandLineParser>
#include <cstdio>
#include <iostream>

#ifdef Q_OS_WIN
#    include <windows.h>
#endif

#include "app.h"

int main(int argc, char* argv[])
{
    // Force C locale to force decimal point
    QLocale::setDefault(QLocale::c());

#ifdef Q_OS_WIN
    // WIN32 GUI subsystem has no console by default; skip streams already redirected.
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        if (GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) == FILE_TYPE_UNKNOWN) {
            freopen("CONOUT$", "w", stdout);
        }
        if (GetFileType(GetStdHandle(STD_ERROR_HANDLE)) == FILE_TYPE_UNKNOWN) {
            freopen("CONOUT$", "w", stderr);
        }
    }
#endif

    QCoreApplication::setOrganizationName("fstl-app");
    QCoreApplication::setOrganizationDomain("https://github.com/fstl-app/fstl");
    QCoreApplication::setApplicationName("fstl");
    QCoreApplication::setApplicationVersion(FSTL_VERSION);

    // Parse --version/-V/--help before constructing App/QApplication, so no
    // window is created and argv[1] isn't treated as a filename for these
    // flags. parse() (not process()) so App's QApplication can still handle
    // its own options (-style, -platform, etc.) unrejected.
    {
        QCoreApplication pre(argc, argv);

        QCommandLineParser parser;
        parser.setApplicationDescription("Fast .stl file viewer");
        QCommandLineOption helpOption = parser.addHelpOption();

        // Add custom version option with both -V (uppercase, POSIX) and --version
        QCommandLineOption versionOption(QStringList{"V", "version"}, "Displays version information.");
        parser.addOption(versionOption);

        // For --help text only; App (app.cpp) reads argv directly for the file.
        parser.addPositionalArgument("file", "STL file to open (optional)", "[file]");

        parser.parse(pre.arguments());

        if (parser.isSet(helpOption)) {
            parser.showHelp(0);
        }

        // Handle version option manually since we use custom -V
        if (parser.isSet(versionOption)) {
            std::cout << QCoreApplication::applicationName().toStdString() << " "
                      << QCoreApplication::applicationVersion().toStdString() << "\n";
            return 0;
        }
    }

    QGuiApplication::setDesktopFileName("fstlapp-fstl.desktop");

    App a(argc, argv);

    return a.exec();
}
