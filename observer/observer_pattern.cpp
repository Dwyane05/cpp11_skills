/*
 * observer观察者模式
 */

#include <list>
#include <iostream>
#include <string>
#include <memory>

class Student;

class Observer
{
public:
    Observer(){}
    virtual ~Observer(){}
    virtual void update(std::string &msg) = 0;

};

class Subject
{
public:
    Subject() {}
    virtual ~Subject(){}

    void attach(std::weak_ptr<Observer> s);
    void detach(std::weak_ptr<Observer> s);
    void notify(std::string &msg);

private:
    std::list<std::weak_ptr<Observer>>    observer_list;
    typedef std::list<std::weak_ptr<Observer>>::iterator    obser_iter;
};

void Subject::attach( std::weak_ptr<Observer> s)
{
    observer_list.push_back(s);
}

//void Subject::detach(std::weak_ptr<Observer> &s)
//{
//    auto ptr = std::find( observer_list.begin(), observer_list.end(), s);
//    if( ptr != observer_list.end() ){
//        observer_list.erase(s);
//    }
//}

void Subject::notify( std::string &msg )
{
    for( obser_iter it = observer_list.begin(); it != observer_list.end(); ){
        std::shared_ptr<Observer> obj(it->lock());
        if( obj ){
            obj->update(msg);
            ++it;
        }else {
            it = observer_list.erase(it);
        }

    }
}


class Student : public Observer
{
public:
    Student() {}
    virtual ~Student() {}

    void update(std::string &msg) override
    {
        std::cout << "Student done " << msg << std::endl;
    }
};

class Teacher : public Subject
{
public:
    Teacher() {}
    virtual ~Teacher(){}

    void assign_homework(std::string &homework);

};

void Teacher::assign_homework( std::string &homework)
{
    notify(homework);
}


int main()
{
    std::shared_ptr<int> tmp_ptr = std::make_shared<int>(12);
    std::cout << *tmp_ptr << std::endl;

    std::shared_ptr<Student> stu1 = std::make_shared<Student>();
    std::weak_ptr<Student> weak_stu1(stu1);

    std::string work_str("math homework");
    std::shared_ptr<Teacher> teacher = std::make_shared<Teacher>();
    {
        std::shared_ptr<Student> stu2 = std::make_shared<Student>();
        std::weak_ptr<Student> weak_stu2(stu2);
        teacher->attach(weak_stu1);
        teacher->attach(weak_stu2);

        teacher->assign_homework(work_str);
    }

    work_str = "chinese homework";
    teacher->assign_homework(work_str);
}
