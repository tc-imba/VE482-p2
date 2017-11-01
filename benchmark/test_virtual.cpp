//
// Created by 38569 on 2017/11/1.
//

class A {
public:
    class Child {
    protected:
        A* ptr = nullptr;
    public:
        virtual A* getPtr() = 0;
    };
};

class B : public A {
public:
    class Child : public A::Child {
        virtual B* getPtr() {
            return dynamic_cast<B*>(ptr);
        }
    };
};

int main() {

}