#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;
class Teacher;
class Student
{
public:
    virtual ~Student() {}
    virtual void DoHomeWork(string str) = 0;
protected:
    // 设为保护，既防止实例化，派生类又可以调用
    Student() {}
};

class Teacher
{
public:
    virtual ~Teacher() {}
    // 向电话簿添加学生
    virtual void Attach(Student* O) {m_Students.push_back(O);}
    // 从电话簿删除学生
    virtual void Detach(Student* O)
    {
        m_Students.erase(remove(m_Students.begin(), m_Students.end(), O), m_Students.end());
    }
    // 通知学生们写作业
    virtual void Notify(string msg)
    {
        // 循环通知学生们去写作业
        for (vector<Student*>::const_iterator iter = m_Students.begin();
        iter != m_Students.end();
        ++iter)
        {
            if (*iter != 0)
            {
                (*iter)->DoHomeWork(msg);
            }
        }
    }
protected:
    // 设为保护，既防止实例化，派生类又可以调用
    Teacher() {}
private:
    vector<Student*> m_Students;
};

// 定义数学老师类，继承Teacher
class MathTeacher : public Teacher
{
    public:
    MathTeacher() {}
    // 数学老师布置作业
    void AssignHomework(string msg) { Notify(msg); }
};

// 定义语文老师类，继承Teacher
class ChineseTeacher : public Teacher
{
    public:
    ChineseTeacher() {}
    // 数学老师布置作业
    void AssignHomework(string msg) { Notify(msg); }
};

class FreshStudent : public Student
{
    public:
    FreshStudent() {}
    virtual ~FreshStudent() {}
    void DoHomeWork(string str) override { cout << "I have done "<< str << endl;}
};

int main()
{
    MathTeacher me;
    ChineseTeacher other_t;
    FreshStudent FS_1;
    FreshStudent FS_2;

    // 两位老师互相不可见，但是是同一批学生
    // 每个老师布置作业虽然不多，但是在学生来看，作业还是很繁重滴
    me.Attach(&FS_1);
    me.Attach(&FS_2);
    other_t.Attach(&FS_1);
    other_t.Attach(&FS_2);

    me.AssignHomework("math homework");
    other_t.AssignHomework("chinese homework");
    return 0;
}
