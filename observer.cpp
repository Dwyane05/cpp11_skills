
#include <algorithm>
#include <list>
#include <stdio.h>

class Subject;

class Observer
{
public:
    virtual ~Observer();
    virtual void update() = 0;

    void observe(Subject* s);

protected:
    Subject* subject_;
};

class Subject
{
public:
    void attach(Observer* s)
    {
        observers_.push_back(s);
    }

    void detach(Observer* s)
    {
        auto iter = std::find( observers_.begin(), observers_.end(), s);
        if( iter != observers_.end() ){
            observers_.erase(iter);
        }
    }

    void notify_observers()
    {
        for (auto obs : observers_ )
        {
            obs->update();
        }
    }

private:
    std::list<Observer*> observers_;
};

Observer::~Observer()
{
  subject_->detach(this);
}

void Observer::observe(Subject* s)
{
  s->attach(this);
  subject_ = s;
}


class Foo : public Observer
{
  void update() override
  {
    printf("Foo::update() %p\n", this);
  }
};

int main()
{
  Foo* p = new Foo;
  Subject subject;
  p->observe(&subject);
  subject.notify_observers();

  Foo* p1 = new Foo;

  delete p;
  p1->observe(&subject);
  delete p1;
  subject.notify_observers();

}
