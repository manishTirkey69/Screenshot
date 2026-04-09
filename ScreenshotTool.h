#ifndef SCREENSHOTTOOL_H
#define SCREENSHOTTOOL_H

#include <QWidget>
#include <QScreen>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QClipboard>
#include <QLabel>
#include <QPainterPath>
#include <QKeyEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QAbstractNativeEventFilter>
#include <QTimer>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class QCheckBox;
class QVBoxLayout;
class QDialogButtonBox;
class QShortcut;

// Native event filter for global hotkeys
class GlobalHotkeyFilter : public QAbstractNativeEventFilter
{
    QObject* m_target;
    const char* m_slot;

public:
    GlobalHotkeyFilter(QObject* target, const char* slot);
    ~GlobalHotkeyFilter();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif

private:
#ifdef Q_OS_WIN
    bool registerHotKeys();
    bool unregisterHotKeys();
    static const int HOTKEY_ID_PRINT = 1;
    static const int HOTKEY_ID_CTRL_PRINT = 2;
    static const int HOTKEY_ID_SHIFT_PRINT = 3;
#endif
};

// Custom QLabel class to handle key events
class PreviewWindow : public QLabel
{
    Q_OBJECT

public:
    PreviewWindow(QWidget *parent = nullptr);
    void setSaveShortcut(const QString& shortcutText);

signals:
    void saveRequested();
    void copyToClipboardRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString m_saveShortcut;
};

class ScreenshotTool : public QWidget
{
    Q_OBJECT

public:
    ScreenshotTool(QWidget *parent = nullptr);
    ~ScreenshotTool();

public slots:
    void startCapture();
    void startCaptureAndSave();
    void startCaptureAndPreview();
    void saveScreenshot();
    void showCapturedImage();
    void copyToClipboard();
    void showShortcutsDialog();
    void showAboutDialog();
    void openConfigFile();
    void quitApplication();
    void startCaptureFromTray();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QPoint startPos;
    QPoint endPos;
    QPoint currentMousePos;
    QRect virtualDesktopGeometry;
    QPixmap fullScreenPixmap;
    QPixmap capturedPixmap;
    bool isCapturing;
    bool hasCapture;
    PreviewWindow* previewWindow;
    QSystemTrayIcon* trayIcon;
    QMenu* trayMenu;
    QSettings settings;
    GlobalHotkeyFilter* hotkeyFilter;
    bool autoStartEnabled;
    QTimer* cursorTracker;
    QAction* autoStartAction;
    QShortcut* printScreenShortcut;
    QString captureClipboardShortcut;
    QString captureSaveShortcut;
    QString capturePreviewShortcut;
    QString cancelCaptureShortcut;
    QString previewSaveShortcut;
    QString captureOverlaySaveShortcut;
    qint64 lastCaptureRequestMs;

    // Capture modes
    enum CaptureMode {
        ClipboardOnly,
        ClipboardAndSave,
        ClipboardAndPreview
    };
    CaptureMode currentCaptureMode;

    void captureFullScreen();
    QString generateFileName();
    void drawZoomArea(QPainter& painter);
    void setupTrayIcon();
    void setupShortcuts();
    void loadSettings();
    void saveSettings();
    QString getConfigFilePath() const;
    void setAutoStart(bool enable);
    bool isAutoStartEnabled() const;
    QString getApplicationPath() const;
    void updateCursorPosition();
    void handleCaptureCompletion();
    void performCapture();
};

#endif // SCREENSHOTTOOL_H
