/*
 * PRBTree.h
 *
 *  Created on: 28 ene. 2018
 *      Author: christian
 */

#ifndef DATASTRUCTURES_SIMPLE_PRBTree_H_
#define DATASTRUCTURES_SIMPLE_PRBTree_H_

#include <vector>


template <class T>
class PVertex{
private:
	T PX, PY;
public:
	PVertex(T _x, T _y):PX(_x),PY(_y){}
	PVertex():PX(0),PY(0){}
	PVertex(const PVertex& _V){
		PX = _V.PX;
		PY  = _V.PY;
	}
	void operator = (PVertex _p2) {
		PX = _p2.X();
		PY = _p2.Y();
	}

	T X(){ return this-PX;}
	T Y(){ return this->PY;}
	void setX(T _x){ PX = _x;}
	void setY(T _y){ PY = _y;}
};

template <class T>
class PEdge{
private:
	PVertex<T> V1, V2;
public:
	PEdge():V1(),V2(){}
	PEdge(PVertex<T> _v1, PVertex<T> _v2){
		V1 = _v1;
		V2 = _v2;
	}
	double slope(){
		double num = V2.Y() - V1.Y();
		double den = V2.X() - V1.X();
		if ( den != 0) return num/den;
		else return 0;
	}
};


class PColor{
public:
	static const bool RED = true;
	static const bool BLACK = false;
};

// Trying to do a persistent LLRB Tree with node copying
template <class key, class value>
class PRBNode {
	private:
		key _key;
		value _value;
		bool color;
		PRBNode<key,value>* left;
		PRBNode<key,value>* right;
		int size;
		int time;
	public:
		PRBNode(key _k, value _val,bool _color,int _size, int time):_key(_k),_value(_val),
			color(_color),left(NULL),right(NULL),size(_size),time(time){}
		PRBNode( PRBNode<key,value>* p, int _time){
			_key = p->Key();
			_value = p->Value();
			color = p->Color();
			left = p->Left();
			right = p->Right();
			size = p->Size();
			time = _time;
		}
		bool Color(){ return color;}
		void setColor(bool _color){ color = _color;}
		value Value(){ return _value;}
		int Size(){ return size;}
		void setSize(int _size){size= _size;}
		void setValue(value _value){ this->_value = _value;}
		key Key(){ return _key;}
		void setKey(key _key){this->_key = _key;}
		void setTime(int _time){ time = _time;}
		int getTime(){return time;}
		PRBNode<key,value>*& Left(){return left;}
		PRBNode<key,value>*& Right(){return right;}
};


template <class key,class value>
class PRBTree {
	private:
		PRBNode<key,value>** timeline; // we conserve a timeline
		int nullCounter; // to print null in the dot file
		int actualTime; // we can not modify the structure before this time
		int numTimes;

		bool isRed(PRBNode<key,value>*);
		int Size(PRBNode<key,value>* x);
		void colorFlip(PRBNode<key,value>*);
		PRBNode<key,value>* insert(PRBNode<key,value>*,key _key,value _value, int time);
		PRBNode<key,value>* erase(PRBNode<key,value>*,key _key);
		PRBNode<key,value>* min(PRBNode<key,value>*);
		PRBNode<key,value>* max(PRBNode<key,value>*);
		PRBNode<key,value>* deleteMin(PRBNode<key,value>*);
		PRBNode<key,value>* deleteMax(PRBNode<key,value>*);
		PRBNode<key,value>* moveRedLeft(PRBNode<key,value>*);
		PRBNode<key,value>* moveRedRight(PRBNode<key,value>*);
		PRBNode<key,value>* rotateLeft(PRBNode<key,value>*);
		PRBNode<key,value>* rotateRight(PRBNode<key,value>*);
		PRBNode<key,value>* balance(PRBNode<key,value>* h);
		void show(PRBNode<key,value>*,int time,std::ostream&);
	public:
		PRBTree():timeline(NULL),nullCounter(0),actualTime(0),numTimes(0){}
		void insert(key _key, value _value, int time); // only insert at actualTime
		value search(key _key, int time);
		bool contains(key _key, int time);
		void erase(key _key);
		void deleteMin();
		key min(int time);
		key max(int time);
		void deleteMax();
		bool isEmpty(int time);
		void show(std::ostream&, int);
};

template <class key, class value>
bool PRBTree<key,value>::isRed(PRBNode<key,value>* x){
	if (x == NULL) return false;
	return x->Color() == Color::RED;
}

template <class key, class value>
int PRBTree<key,value>::Size(PRBNode<key,value>* x) {
	if (x == NULL) return 0;
	return x->Size();
}

template <class key, class value>
bool PRBTree<key,value>::isEmpty(int time) {
	return timeline[actualTime-1] == NULL;
}


template <class key, class value>
void PRBTree<key,value>::colorFlip(PRBNode<key,value>* h){
	h->setColor(!(h->Color()));
	h->Left()->setColor(!(h->Left()->Color()));
	h->Right()->setColor(!(h->Right()->Color()));
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::rotateLeft(PRBNode<key,value>* h) {
	PRBNode<key,value>* x = h->Right();
	h->Right() = x->Left();
	x->Left() = h;
	x->setColor(x->Left()->Color());
	x->Left()->setColor(Color::RED);
	x->setSize(h->Size());
	h->setSize(Size(h->Left()) + Size(h->Right()) + 1);
	return x;
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::rotateRight(PRBNode<key,value>* h) {
	PRBNode<key,value>* x = h->Left();
	h->Left() = x->Right();
	x->Right() = h;
	x->setColor(x->Right()->Color());
	x->Right()->setColor(Color::RED);
	x->setSize(h->Size());
	h->setSize(Size(h->Left()) + Size(h->Right()) + 1);
	return x;
}

template <class key, class value>
void PRBTree<key,value>::insert(key _key, value _value, int time){
	if (time >= actualTime) {
		if(!timeline) {
			std::cout <<"Initializing time ";
			timeline = new PRBNode<key,value>*[5]; // time begins!!!
			for (int i = 0; i < 5; i++){
				timeline[i] = NULL;
				std::cout<<".";
			}
			std::cout<<" Terminated."<< std::endl;
		}
		if(numTimes == 0 || actualTime == time) {
			std::cout<<"Working at same line of time." << std::endl;
			timeline[time-1] = insert(timeline[time - 1],_key,_value,time);
			timeline[time-1]->setColor(Color::BLACK);
		} else {
			std::cout<<"Working at different line of time." << std::endl;
			timeline[time-1] = insert(new PRBNode<key,value>(timeline[time - 2],time),_key,_value,time);
			timeline[time-1]->setColor(Color::BLACK);
		}
		if (actualTime != time) numTimes++;
		actualTime = time;

	}
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::insert(PRBNode<key,value>* h,key _key, value _value, int time){
	if (!h) return new PRBNode<key,value>(_key,_value,Color::RED,1,time);

	if (_key < h->Key()) {
		if (h->Left() != NULL) {
			if(h->Left()->getTime() != time) {
				h->Left() = new PRBNode<key,value>(h->Left(),time);
				std::cout<<"Node copied with key:" << h->Left()->Value() << std::endl;
			}
		}
		h->Left() = insert(h->Left(),_key,_value, time);
	}
	else if (_key > h->Key()) {
		if (h->Right() != NULL) {
			if (h->Right()->getTime() != time) {
				h->Right() = new PRBNode<key,value>(h->Right(),time);
				std::cout<<"Node copied with key:" << h->Right()->Value() << std::endl; ;
			}
		}
		h->Right() = insert(h->Right(),_key,_value, time);
	}
	else {
		h->setValue(_value);
	}

	if (isRed(h->Right()) && !(isRed(h->Left()))) h = rotateLeft(h);
	if (isRed(h->Left()) && isRed(h->Left()->Left())) h = rotateRight(h);
	if (isRed(h->Left()) && isRed(h->Right())) colorFlip(h);

	h->setSize(Size(h->Left()) + Size(h->Right()) + 1);
	return h;
}


template <class key, class value>
value PRBTree<key,value>::search(key _key, int time) {
	if (_key != NULL) {
		if (!timeline[time-1]) return NULL;

		PRBNode<key,value>* x = timeline[time-1];
		while (x) {
			if (x->Key() > _key) x = x->Left();
			else if (x->Key() < _key) x =x->Right();
			else return x->Value();
		}
	}
	return NULL;
}

template <class key, class value>
bool PRBTree<key,value>::contains(key _key, int time) {
	return search(_key,time) != NULL;
}
template <class key, class value>
void PRBTree<key,value>::erase(key _key) {
	std::cout<<"Erasing " <<_key<<"..."<<std::endl;
	if (_key != NULL && contains(_key, actualTime)) {
		if (!isRed(timeline[actualTime-1]->Left()) && !isRed(timeline[actualTime-1]->Right())) timeline[actualTime-1]->setColor(Color::RED);
		timeline[actualTime-1] = erase(timeline[actualTime-1],_key);
		if (!isEmpty(actualTime)) timeline[actualTime-1]->setColor(Color::BLACK);
	}
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::erase(PRBNode<key,value>* h, key _key) {

	if (_key < h->Key()) {
		if (!isRed(h->Left()) && !isRed(h->Left()->Left())) {
			h = moveRedLeft(h);
		}
		h->Left() = erase(h->Left(), _key);
	} else {
		if (isRed(h->Left())) h = rotateRight(h);
		if ((_key == h->Key()) && (h->Right() == NULL)) return NULL;
		if (!isRed(h->Right()) && !isRed(h->Right()->Left())) h = moveRedRight(h);
		if (_key == h->Key()) {
			PRBNode<key,value> *x = min(h->Right());
			h->setKey(x->Key());
			h->setValue(x->Value());
			h->Right() = deleteMin(h->Right());
		} else {
			h->Right() = erase(h->Right(),_key);
		}
	}
	return balance(h);
}


template <class key, class value>
void PRBTree<key,value>::deleteMin(){
	if (!isEmpty(actualTime)) {
		if (!isRed(timeline[actualTime-1]->Left()) && !isRed(timeline[actualTime-1]->Right())) {
			timeline[actualTime-1]->setColor(Color::RED);
		}
		timeline[actualTime-1] = deleteMin(timeline[actualTime-1]);
		if (!isEmpty(actualTime)) timeline[actualTime-1]->setColor(Color::BLACK);
	}
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::deleteMin(PRBNode<key,value>* h) {
	if (h->Left() == NULL) {
		return NULL;
	}
	if (!isRed(h->Left()) && !isRed(h->Left()->Left())) {
		h = moveRedLeft(h);
	}
	h->Left() = deleteMin(h->Left());
	return balance(h);
}

template <class key, class value>
void PRBTree<key,value>::deleteMax() {
	if (!isEmpty(actualTime)) {
		if (!isRed(timeline[actualTime-1]) && !isRed(timeline[actualTime-1]->Right())) {
			timeline[actualTime-1]->setColor(Color::RED);
		}
		timeline[actualTime-1] = deleteMax(timeline[actualTime-1]);
		if (!isEmpty(actualTime)) timeline[actualTime-1]->setColor(Color::BLACK);
	}
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::deleteMax(PRBNode<key,value>* h) {
	if (isRed(h->Left())) h = rotateRight(h);
	if (h->Right() == NULL) return NULL;
	if (!isRed(h->Right()) && !isRed(h->Right()->Left())) h = moveRedRight(h);
	h->Right() = deleteMax(h->Right());
	return balance(h);
}

template <class key, class value>
key PRBTree<key,value>::min(int _time) {
	if (!isEmpty(actualTime)) {
		return min(timeline[_time-1])->Key();
	}
	return NULL;
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::min(PRBNode<key,value>* x) {
	if (x->Left() == NULL) return x;
	return min(x->Left());
}


template <class key, class value>
key PRBTree<key,value>::max(int _time) {
	if (!isEmpty(actualTime)) {
		return max(timeline[_time-1])->Key();
	}
	return NULL;
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::max(PRBNode<key,value>* x) {
	if (x->Right() == NULL) return x;
	return max(x->Right());
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::moveRedLeft(PRBNode<key,value>* h) {
	colorFlip(h);
	if (isRed(h->Right()->Left())) {
		h->Right() = rotateRight(h->Right());
		h = rotateLeft(h);
		colorFlip(h);
	}
	return h;
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::moveRedRight(PRBNode<key,value>* h) {
	colorFlip(h);
	if (isRed(h->Left()->Left())) {
		h = rotateRight(h);
		colorFlip(h);
	}
	return h;
}

template <class key, class value>
PRBNode<key,value>* PRBTree<key,value>::balance(PRBNode<key,value>* h) {
	if (isRed(h->Right())) h = rotateLeft(h);
	if (isRed(h->Left()) && isRed(h->Left()->Left())) h = rotateRight(h);
	if (isRed(h->Left()) && isRed(h->Right())) colorFlip(h);
	h->setSize(Size(h->Left()) + Size(h->Right()) +1);
	return h;
}

template <class key,class value>
void PRBTree<key,value>::show(PRBNode<key,value>* pointer,int tempTime,std::ostream& out){
	if (pointer->Color() == Color::RED)
		out<<"\t"<<pointer->Value()<<" [shape=circle color=red];\n";
	else out<<"\t"<<pointer->Value()<<" [shape=circle];\n";
	if (pointer->Left() != NULL) {
		out <<"\t"<< pointer->Value() << "->"
				<< pointer->Left()->Value()<<";\n";
		show(pointer->Left(),tempTime,out);
	} else {
		//out <<"\t"<< "null"<<nullCounter<< "[shape=point];\n";
		//out <<"\t"<< pointer->Value() << "->"
		//		<< "null"<<nullCounter++<<";\n";
	}
	if (pointer->Right() != NULL) {
		out <<"\t"<< pointer->Value() << "->"
				<< pointer->Right()->Value()<<";\n";
		show(pointer->Right(),tempTime,out);
	} else {
		//out <<"\t"<< "null"<<nullCounter<< "[shape=point];\n";
		//out <<"\t"<< pointer->Value() << "->"
		//		<< "null"<<nullCounter++<<";\n";
	}
}

template <class key,class value>
void PRBTree<key,value>::show(std::ostream& out, int time){
	std::cout << "Total times:"<< numTimes << std::endl;
	std::cout<< "Printing line of time: "<<time<<std::endl;
	out << "digraph BST {\n\tnode [fontname=\"Arial\"];\n";
	//if (root) show(root,out);
//	int tempTime = 0;
//	while (timeline[tempTime] != NULL) {
//		std::cout << "Printing time line "<< tempTime <<" with key:" << timeline[tempTime]->Value() << std::endl;
//		show(timeline[tempTime++],tempTime,out);
//	}
	show(timeline[time-1],time,out);
	out<<"}\n";
}

#endif /* DATASTRUCTURES_SIMPLE_PRBTree_H_ */
