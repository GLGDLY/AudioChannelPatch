#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audio.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <qnamespace.h>
#include <qslider.h>
#include <vector>


QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow* ui;
	Audio* audio;

	QVBoxLayout* layout;
	std::vector<QHBoxLayout*> channel_layouts;
	std::vector<QLabel*> channel_labels;
	std::vector<QLineEdit*> channel_lineedits;
	std::vector<QSlider*> channel_sliders;

	void update_layout(void);
	void update_audio(void);

private slots:
	void reload_audio(void);
	void reset_audio_levels(void);
	void setup_scroll_area(void);
	void save_audio_levels(void);
};
#endif // MAINWINDOW_H
