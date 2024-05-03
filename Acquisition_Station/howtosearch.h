#ifndef HOWTOSEARCH_H
#define HOWTOSEARCH_H

#include <QDialog>

namespace Ui {
class HowToSearch;
}

class HowToSearch : public QDialog
{
    Q_OBJECT

public:
    explicit HowToSearch(int type,QString userid,bool videojur,QWidget *parent = nullptr);
    ~HowToSearch();
signals:
    void gotosearch_by_files_type(int type,QString userid,bool videojur);

    void gotosearch_by_date(int type,QString userid,bool videojur);

    void gotosearch_by_staff_name(int type,QString userid,bool videojur);

    void goback();
private slots:
    void on_pushButton_search_by_files_type_clicked();

    void on_pushButton_search_by_date_clicked();

    void on_pushButton_search_by_staff_name_clicked();

    void on_btn_back_clicked();

private:
    Ui::HowToSearch *ui;
    int type = -1;
    QString userid;
    bool videojur;
};

#endif // HOWTOSEARCH_H
