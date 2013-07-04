#include "cyber.h"
#include <QApplication>

int main (int argc, char ** argv)
{
    int c;
    QApplication * b = new QApplication (argc, argv);
    Cyber * a = new Cyber();
    printf("Java sucks!\n");
    printf ("Engines is going to run\n");
    a->turn(15.0F);

    scanf("%d", &c);
    a->stop();
    b->exit(0);
    return 0;
}
