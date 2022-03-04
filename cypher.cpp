#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::validateState MainWindow::validateChar(QChar character)
{
    //Musi być, żeby validatory poprawnie działały, bo biorą zmienne przez referencje i nawet je modyfikują
    QString t = character;
    int     i = 0;

    if (charSpecialValidator->validate(t, i) == QValidator::Acceptable)
        return Special;

    //Reset, bo validatory je zmodyfikowały
    t = character;
    i = 0;

    if (charValidator->validate(t, i) == QValidator::Acceptable)
        return Valid;

    return Invalid;
}

//Dwie poniższe funckje zwracają znak z szyfrowania/deszyfrowania. Dodatkowo jako parametr biorą iterację naszego programu, co w wielu szyfrach się przydaje (tutaj akurat nie)
QChar MainWindow::cypherStep(QChar character)
{
    //ROT3
    MainWindow::validateState state = validateChar(character);
    if (state == Special)
        return character;

    if (state == Valid)
        return QChar('A' + (character.toLatin1() - 'A' + 3) % ('Z' - 'A' + 1));

    return '?';
}

QChar MainWindow::decypherStep(QChar character)
{
    //ROT3
    MainWindow::validateState state = validateChar(character);
    if (state == Special)
        return character;

    if (state == Valid)
    {
        int val = static_cast<int>(character.toLatin1() - 'A' - 3) % ('Z' - 'A' + 1);
        return QChar(((val >= 0) ? ('A' + val) : ('Z' + val + 1)));
    }
    return '?';
}

void MainWindow::stepVisualize(QChar oldChar, QChar newChar)
{
    //Aktualizacja paska postępu
    ui->visProgressBar->setValue(iteration);
    ui->secondText->setPlainText(processedText);
    ui->visTable->insertColumn(ui->visTable->columnCount());
    ui->visTable->setItem(0, ui->visTable->columnCount() - 1,     new QTableWidgetItem(oldChar));
    //Wizualizacja w 3 krokach, jak zwiększamy znak
    if (validateChar(oldChar) == Valid)
    {
        ui->visTable->setItem(1, ui->visTable->columnCount() - 1, new QTableWidgetItem(oldChar));
        visIteration = 0;
        visTimer.start();
    }
    else //Jak jest niedozwolony lub specjalny znak
        ui->visTable->setItem(1, ui->visTable->columnCount() - 1, new QTableWidgetItem(newChar));
}

void MainWindow::stepSubVisualize()
{
    char c = ui->visTable->item(1, iteration - 1)->text().at(0).toLatin1();
    //Zmiana znaku
    if (cypherMode)
    {
        int val = static_cast<int>(c - 'A' - 1) % ('Z' - 'A' + 1);
        ui->visTable->item(1, iteration - 1)->setText(QChar(((val >= 0) ? ('A' + val) : ('Z' + val + 1))));
    }
    else
        ui->visTable->item(1, iteration - 1)->setText(QChar('A' + (c - 'A' + 1) % ('Z' - 'A' + 1)));
    //Różnica między znakami = 3, wtedy koniec
    if (++visIteration == VIS_STEPS)
        visTimer.stop();
}

void MainWindow::execute()
{
    //Podczas działania programu, zabraniamy modyfikować CheckBox
    ui->modeCheck->setEnabled(false);

    //Zmieniamy funkcjonalność przycisku na stop
    ui->executeButton->setText("Stop");
    ui->executeButton->disconnect();
    connect(ui->executeButton, &QPushButton::clicked, this, &MainWindow::stopExecuting);

    //Najpierw trzeba posprzątać po poprzednim uruchomieniu programu, o ile to się wydarzyło
    ui->secondText->clear();
    ui->visTable->setColumnCount(0);
    ui->visProgressBar->setValue(-1);
    iteration = 0;

    //Ustawiamy maksymalne iteracje programu na liczbę znaków w naszym tekście. Potrzebujemy tej informacji do ustalenia maksu dla paska progresu
    maxIterations = ui->firstText->toPlainText().size();
    ui->visProgressBar->setMaximum(maxIterations);

    if (!maxIterations)
        return;
    //Zbieramy tekst do przetworzenia, który zawsze znajduje się w tym samym polu tekstowym w programie
    text            = ui->firstText->toPlainText();
    processedText   = "";

    //Timer, który wywołuje executing co określony czas
    executeTimer.start();
}

//Funkcja wywoływana w określonych interwałach
void MainWindow::executing()
{
    //Czasami jest tak, że wywołujemy następny krok, ale wizualizacja poprzedniego jeszcze się nie skończyła, więc musimy to nadrobić
    if (iteration != 0)
    {
        while (visIteration < VIS_STEPS)
            stepSubVisualize();
    }
    //Przetwarzamy kolejny znak i dodajemy go na koniec [processedText]
    QChar   character = text.at(iteration),
            processedChar;

    if (cypherMode)
        processedChar = decypherStep(character);
    else
        processedChar = cypherStep(character);

    processedText.append(processedChar);

    //Wizualizacja
    ++iteration;
    stepVisualize(character, processedChar);

    if (iteration == maxIterations)
        stopExecuting();
}

void MainWindow::stopExecuting()
{
    executeTimer.stop();

    //Przywracamy funkcjonalność przycisku
    ui->executeButton->setText(cypherMode ? "Deszyfruj" : "Szyfruj");
    ui->executeButton->disconnect();
    connect(ui->executeButton, &QPushButton::clicked, this, &MainWindow::execute);

    //Przypisujemy przetworzony tekst do pola na wyjście programu
    ui->secondText->setPlainText(processedText);

    //Koniec roboty, więc z powrotem pozwalamy zmieniać tryb szyfrowania
    ui->modeCheck->setEnabled(true);
}
