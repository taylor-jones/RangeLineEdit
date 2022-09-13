#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow{

    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    void setupDMSWidget();
    void setupDoubleWidget();
    void setupPhoneWidget();
    void setupTimeWidget();

    ~MainWindow();

    QTabWidget* m_tabWidget = nullptr;
    QWidget*    m_dmsWidget = nullptr;
    QWidget*    m_doubleWidget = nullptr;
    QWidget*    m_phoneWidget = nullptr;
    QWidget*    m_timeWidget = nullptr;

};

#endif // MAINWINDOW_H
