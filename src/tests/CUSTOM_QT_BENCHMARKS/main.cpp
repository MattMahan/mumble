#include <iostream>
#include <sstream>
#include <QList>
#include <QVector>
#include <QTime>
#include <vector>
#include <QtAlgorithms>
#include <string>
#include "Element.h"

using namespace std;

#define NUM_ITEMS 1000000
#define NE 3000000
#define NL 100000000

Element::Element(){
        value_one = value_two = value_three = 123456789;
}

Element::Element(int a, int b, int c, std::string d, std::string e, std::string f){
        value_one = a;
        value_two = b;
        value_three = c;
        string_one = d;
        string_two = e;
        string_three = f;
}

bool lessThan(const Element & e1, const Element & e2){
        if(e1.value_one < e2.value_one) return true;
        else return false;
}

int randInt(int low, int high){
        // Random number between low and high
        return qrand() % ((high - 1) - low) + low;
}

std::string compareTimes(const double & QList_time, const double & QVector_time){
        std::ostringstream s;
        if(QList_time > QVector_time){
                        double diff = QList_time - QVector_time;
                        int pct_diff = (diff/QList_time)*100;
                        s << "QVector outperformed QList by " << diff << "ms (~ " << pct_diff << "% )";
        }
        else if(QVector_time > QList_time){
                int diff = QVector_time - QList_time;
                int pct_diff = ((double)diff/QVector_time)*100;
                s << "QList outperformed QVector by " << diff << "ms (~ " << pct_diff << "% )";
        }
        else s << "QList and QVector had equal performance.";
        return s.str();
}


int main()
{

        // Seed the random number generator
        QTime time = QTime::currentTime();
        qsrand((uint)time.msec());
        
        // Fill array with random ints        
        // (These will be used to fill QList and QVector)
        int array[NUM_ITEMS];
        for(unsigned i = 0; i < NUM_ITEMS; i++){
                array[i] = randInt(0,1000000);
        }
        // Store numbers for search later
        int initialNum = array[NUM_ITEMS/4];
        int midNum = array[NUM_ITEMS/2];
        int lateNum = array[(NUM_ITEMS/8)*7];
        int searchNum;

        // Initialize a QTime object
        QTime currentTime;

		int QList_insertion_time, QList_search_time, QList_sort_time, QList_iteration_time;
		int QVector_insertion_time, QVector_search_time, QVector_sort_time, QVector_iteration_time;

        cout << "=====[ Tests using large objects (complex) ]=====" << endl;

        // Fill up the QList with Elements
        QList<Element> list;
        currentTime = QTime::currentTime();
        for(unsigned i = 0; i < NUM_ITEMS; i++){
                Element temp(array[i],array[i],array[i],"qwertyuiop","asdfghjkl","zxcvbnm");
                list.append(temp);
        }
        // Report outcome to terminal
        QList_insertion_time = currentTime.msecsTo(QTime::currentTime());
        cout << "QList insertion: " << QList_insertion_time << " ms" << endl;

        currentTime = QTime::currentTime();
        // Run selection sort on list
        QList<Element>::iterator l_it;
        for(unsigned i=0;i<3;i++){
                if(i==0) searchNum = initialNum;
                else if(i==1) searchNum = midNum;
                else if(i==2) searchNum = lateNum;
                for(l_it = list.begin(); l_it != list.end(); l_it++){
                        if((*l_it).value_one == searchNum) break;
                }
        }
        QList_search_time = currentTime.msecsTo(QTime::currentTime());
        cout << "QList search: " << QList_search_time << " ms" << endl;

        currentTime = QTime::currentTime();
        // Run qSort on QList
        qSort(list.begin(),list.end(),lessThan);
        QList_sort_time = currentTime.msecsTo(QTime::currentTime());
        cout << "QList qSort: " << QList_sort_time << " ms" << endl;

        // Fill up the vector with Elements
        QVector<Element> vector;
        currentTime = QTime::currentTime();
        for(unsigned i = 0; i < NUM_ITEMS; i++){
                Element temp(array[i],array[i],array[i],"qwertyuiop","asdfghjkl","zxcvbnm");
                vector.push_back(temp);
        }
        // Report outcome to terminal
        QVector_insertion_time = currentTime.msecsTo(QTime::currentTime());
        cout << "QVector insertion: " << QVector_insertion_time << " ms" << endl;

        // Fill up the vector with Elements
        QVector<Element> vector_2;
        vector_2.reserve(NUM_ITEMS);
        int QVector_insertion_time_two;
        currentTime = QTime::currentTime();
        for(unsigned i = 0; i < NUM_ITEMS; i++){
                Element temp(array[i],array[i],array[i],"qwertyuiop","asdfghjkl","zxcvbnm");
                vector_2.push_back(temp);
        }
        // Report outcome to terminal
        QVector_insertion_time_two = currentTime.msecsTo(QTime::currentTime());
        cout << "QVector insertion (with pre-allocation): " << QVector_insertion_time_two << " ms" << endl;

        currentTime = QTime::currentTime();        
        //Run selection sort on vector
        QVector<Element>::iterator v_it;
        for(unsigned i=0;i<3;i++){
                if(i==0) searchNum = initialNum;
                else if(i==1) searchNum = midNum;
                else if(i==2) searchNum = lateNum;
                for(v_it = vector.begin(); v_it != vector.end(); v_it++){
                        if((*v_it).value_one == searchNum) break;
                }
        }
        QVector_search_time = currentTime.msecsTo(QTime::currentTime());
        cout << "QVector search: " << QVector_search_time << " ms" << endl;

        currentTime = QTime::currentTime();        
        // Run qSort on vector
        qSort( vector.begin(), vector.end(), lessThan );
        QVector_sort_time = currentTime.msecsTo(QTime::currentTime());
        cout << "QVector qSort: " << QVector_sort_time << " ms" << endl;

        // REPORT RESULT STATISTICS
        cout << "RESULT [ Large object Insertion ]: " << compareTimes(QList_insertion_time,QVector_insertion_time) << endl;
        cout << "RESULT [ Non-movable object Linear search ]: " << compareTimes(QList_search_time,QVector_search_time) << endl;
        cout << "RESULT [ Non-movable object qSort ]: " << compareTimes(QList_sort_time,QVector_sort_time) << endl;

/************************************************************************************





*************************************************************************************/


        // Now running benchmarks as inspired by Collections.cpp...
        cout << "=====[ Tests using ints (simple) ]=====" << endl;
        
        int sum;
        QTime middleTime;
        QTime lastTime;

        currentTime = QTime::currentTime();
        QList<int> s;
        for(int i=0;i<NE;i++){
                s.append(i);
        }
        sum = 0;
        middleTime = QTime::currentTime();
        for(int j=0;j<NL;j++){
                foreach(int v, s){
                        sum += v;
                }
        }
        // Sort the list
        lastTime = QTime::currentTime();
        qSort( s );
		QList_insertion_time = currentTime.msecsTo(middleTime);
		QList_iteration_time = middleTime.msecsTo(lastTime);
		QList_sort_time = lastTime.msecsTo(QTime::currentTime());
        cout << "QList insertion: " << QList_insertion_time << " ms" << endl;
        cout << "QList iteration: " << QList_iteration_time << " ms" << endl;
        cout << "QList sort: " << QList_sort_time << " ms" << endl;

        currentTime = QTime::currentTime();
        QVector<int> qv;
        //qv.reserve(NE);
        for(int i=0;i<NE;i++){
                qv.append(i);
        }
        sum = 0;
        middleTime = QTime::currentTime();
        for(int j=0;j<NL;j++){
                foreach(int v, qv){
                        sum += v;
                }
        }
        // Sort the QVector
        lastTime = QTime::currentTime();
        qSort( qv );
		QVector_insertion_time = currentTime.msecsTo(middleTime);
		QVector_iteration_time = middleTime.msecsTo(lastTime);
		QVector_sort_time = lastTime.msecsTo(QTime::currentTime());
        cout << "QVector insertion: " << QVector_insertion_time << " ms" << endl;
        cout << "QVector iteration: " << QVector_iteration_time << " ms" << endl;
        cout << "QVector sort: " << QVector_sort_time << " ms" << endl;

		// REPORT RESULT STATISTICS
        cout << "RESULT [ Movable object Insertion ]: " << compareTimes(QList_insertion_time,QVector_insertion_time) << endl;
        cout << "RESULT [ Movable object Iteration ]: " << compareTimes(QList_iteration_time,QVector_iteration_time) << endl;
        cout << "RESULT [ Movable object qSort ]: " << compareTimes(QList_sort_time,QVector_sort_time) << endl;

        currentTime = QTime::currentTime();
        std::vector<int> sv;
        for (int i=0;i<NE;i++){
                sv.push_back(i);
                sum = 0;
                middleTime = QTime::currentTime();
                int sz = sv.size();
                for (int j=0;j<NL;j++){
                        for (int i=0;i<sz;i++){
                                sum += sv[i];
                        }
                }
        }
        cout << "stl::vector insertion: " << currentTime.msecsTo(middleTime) << " ms" << endl;
        cout << "stl::vector iteration: " << middleTime.msecsTo(QTime::currentTime()) << " ms" << endl;

        currentTime = QTime::currentTime();
        int a[NE];
        for (int i=0;i<NE;i++){
                a[i]=i;
                sum = 0;
                for (int j=0;j<NL;j++){
                        for (int i=0;i<NE;i++){
                                sum += a[i];
                        }
                }
        }
        cout << "Plain array total: " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;
        
}


