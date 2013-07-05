#include "cyber.h"
#include <QApplication>

int main (int argc, char ** argv)
{
    QApplication * a = new QApplication (argc, argv);
    Cyber * b = new Cyber();
    printf("Java sucks!\n");
    printf ("Engines is going to run\n");
    b->turn(15.0F);
    return a->exec();
}
