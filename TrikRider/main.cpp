#include "cyber.h"
#include <QCoreApplication>

int main (int argc, char ** argv)
{
    QCoreApplication * a = new QCoreApplication (argc, argv);
    Cyber * b = new Cyber();
    printf("Java sucks!\n");
    printf ("Engines is going to run\n");
    b->turn(15.0F);
    return a->exec();
}
