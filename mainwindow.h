#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QRegularExpressionValidator>
#include <QFileDialog>
#include <QMessageBox>

#define ALPHABET        "[A-Z \t\n]" //REGEX
#define SPECIAL         "[ \t\n]"
#define KEY_ALPHABET    "[A-Z]"
#define STEP_TIME       160
#define VIS_STEPS       3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    enum validateState
    {
        Invalid,
        Valid,
        Special
    };

    void            execute         ();
    void            executing       ();
    void            stopExecuting   ();

    QChar           cypherStep      (QChar character);
    QChar           decypherStep    (QChar character);
    validateState   validateChar    (QChar character);
    void            stepVisualize   (QChar oldChar, QChar newChar);
    void            stepSubVisualize();

    void            validateInput   ();

    void            setupGuiLabels  (int state);

    void            saveFile        ();
    void            saveAsFile      ();
    void            loadFile        ();

    unsigned        iteration       = 0,
                    visIteration    = 0,
                    maxIterations   = 0;

    QString         text            = "",
                    processedText   = "",
                    savedFileName   = "";

    bool            cypherMode      = false; //false - szyfrowanie, true - deszyfrowanie

    QValidator      *charValidator;
    QValidator      *charSpecialValidator;

    QTimer          executeTimer,
                    visTimer;
};
