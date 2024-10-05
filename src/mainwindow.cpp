#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QPixmap>
#include <QStyleFactory>
#include <QStyleOption>
#include <QtLogging>
#include <qalgorithms.h>


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, audio(nullptr)
	, layout(nullptr)
	, channel_layouts()
	, channel_labels()
	, channel_lineedits()
	, channel_sliders() {
	ui->setupUi(this);

	this->setWindowTitle(tr("Audio Channel Patch"));
	this->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
	this->setFixedSize(this->size());
	this->setStyleSheet("QMainWindow { background-color: #f0f0f0; }");

	// Reload btn
	connect(ui->ReloadBTN, &QPushButton::clicked, this, &MainWindow::reload_audio);
	ui->ReloadBTN->setStyle(QStyleFactory::create("Fusion"));

	// Reset btn
	connect(ui->ResetBTN, &QPushButton::clicked, this, &MainWindow::reset_audio_levels);
	ui->ResetBTN->setStyle(QStyleFactory::create("Fusion"));

	// Save btn
	connect(ui->SaveBTN, &QPushButton::clicked, this, &MainWindow::save_audio_levels);
	ui->SaveBTN->setStyle(QStyleFactory::create("Fusion"));

	// scroll area
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setWidget(ui->scrollAreaWidgetContents);
	ui->scrollArea->setStyleSheet("QScrollArea { background-color: #f0f0f0; border-radius: 10px; }");

	// set up the scroll area to channel level sliders, with textbox beside
	this->setup_scroll_area();
}

MainWindow::~MainWindow() {
	delete ui;
	if (this->audio != nullptr) {
		delete this->audio;
	}
}

void MainWindow::update_layout(void) {
	if (this->audio == nullptr) {
		this->update_audio();
	}
	for (int i = 0; i < this->audio->get_channels(); i++) {
		if (this->channel_sliders.size() <= i || this->channel_lineedits.size() <= i) {
			this->setup_scroll_area();
			return;
		}
		this->channel_sliders[i]->setValue(this->audio->get_channel_vol_level_scalar(i) * 100);
		this->channel_lineedits[i]->setText(QString::number(this->channel_sliders[i]->value()));
	}
}

void MainWindow::update_audio(void) {
	qDebug() << "Updating audio obj";
	if (this->audio != nullptr) {
		delete this->audio;
	}
	this->audio = new Audio();
}

void MainWindow::reload_audio(void) {
	qDebug() << "Reloading audio";
	this->update_audio();
	this->setup_scroll_area(); // may have changed channel count
	QMessageBox::information(this, tr("Reload"), tr("Audio has been reloaded"));
}

void MainWindow::reset_audio_levels(void) {
	qDebug() << "Resetting audio levels";
	if (this->audio == nullptr) {
		this->update_audio();
	}
	this->audio->reset_all_channel_vol_level_scalar();
	this->update_layout();
	QMessageBox::information(this, tr("Reset"), tr("Audio levels have been reset"));
}


void MainWindow::setup_scroll_area(void) {
	qDebug() << "Setting up scroll area";
	if (this->audio == nullptr) {
		this->update_audio();
	}

	// create a new layout
	if (this->layout == nullptr) {
		this->layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
		ui->scrollAreaWidgetContents->setLayout(layout);
	} else {
		for (auto& layout : this->channel_layouts) {
			delete layout;
		}
		this->channel_layouts.clear();
		for (auto& label : this->channel_labels) {
			delete label;
		}
		this->channel_labels.clear();
		for (auto& lineEdit : this->channel_lineedits) {
			delete lineEdit;
		}
		this->channel_lineedits.clear();
		for (auto& slider : this->channel_sliders) {
			delete slider;
		}
		this->channel_sliders.clear();
	}

	for (int i = 0; i < this->audio->get_channels(); i++) {
		QHBoxLayout* hlayout = new QHBoxLayout();
		QLabel* label = new QLabel("Channel " + QString::number(i));
		QSlider* slider = new QSlider(Qt::Horizontal);
		QLineEdit* lineEdit = new QLineEdit();

		slider->setMinimum(0);
		slider->setMaximum(100);
		slider->setStyleSheet(
			"QSlider::groove:horizontal { border: 1px solid #bbb; background: white; height: 10px; border-radius: 4px; "
			"}  QSlider::sub-page:horizontal { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #66e, "
			"stop: 1 #bbf); background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #bbf, stop: 1 #55f); "
			"border: 1px solid #777; height: 10px; border-radius: 4px; }  QSlider::add-page:horizontal {background: "
			"#fff; border: 1px solid #777; height: 10px; border-radius: 4px; }  QSlider::handle:horizontal { "
			"background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc); border: 1px solid #777; "
			"width: 13px; margin-top: -2px; margin-bottom: -2px; border-radius: 4px; }  "
			"QSlider::handle:horizontal:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fff, "
			"stop:1 #ddd); border: 1px solid #444; border-radius: 4px; }  QSlider::sub-page:horizontal:disabled { "
			"background: #bbb; border-color: #999; }  QSlider::add-page:horizontal:disabled { background: #eee; "
			"border-color: #999; }  QSlider::handle:horizontal:disabled { background: #eee; border: 1px solid #aaa; "
			"border-radius: 4px; }");
		slider->setValue(this->audio->get_channel_vol_level_scalar(i) * 100);

		lineEdit->setValidator(new QIntValidator(0, 100));
		lineEdit->setAlignment(Qt::AlignCenter);
		lineEdit->setStyleSheet("QLineEdit { border: 1px solid; border-radius: 5px; }");
		lineEdit->setText(QString::number(slider->value()));

		connect(slider, &QSlider::valueChanged, [lineEdit, this, i](int value) {
			lineEdit->setText(QString::number(value));
			this->audio->set_channel_vol_level_scalar(i, value / 100.0f);
		});
		connect(lineEdit, &QLineEdit::textChanged, [slider, this, i](const QString& text) {
			slider->setValue(text.toInt());
			this->audio->set_channel_vol_level_scalar(i, text.toInt() / 100.0f);
		});

		hlayout->addWidget(label, 1);
		hlayout->addWidget(slider, 2);
		hlayout->addWidget(lineEdit, 1);

		this->layout->addLayout(hlayout);

		this->channel_layouts.push_back(hlayout);
		this->channel_labels.push_back(label);
		this->channel_lineedits.push_back(lineEdit);
		this->channel_sliders.push_back(slider);
	}

	qApp->processEvents();
}

void MainWindow::save_audio_levels(void) {
	qDebug() << "Saving audio levels";
	if (this->audio == nullptr) {
		this->update_audio();
	}
	this->audio->save_ori_level();
	QMessageBox::information(this, tr("Save"), tr("Audio levels have been saved"));
}