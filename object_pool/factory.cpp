#include <iostream>
#include <memory>
#include <map>
#include "../inc/noncopyable.h"
//#include "../inc/Mutex.h"

class Stock : lmp_tool::noncopyable
{
public:
    Stock(const std::string &val)
    :value_(val)
    {
        std::cout << "Stock[" << this << "]: " << value_ <<std::endl;
    }
    ~Stock()
    {
        std::cout << "~Stock[" << this << "]: " << value_ <<std::endl;
    }
    std::string key()
    {
        return value_;
    }

private:
    std::string value_;
};

namespace version1
{

class StockFactory : lmp_tool::noncopyable
{
public:
    StockFactory() {}
    std::shared_ptr<Stock> get(const std::string &key)
    {
//        lmp_tool::MutexLockGuard lock(mutex_);
        std::shared_ptr<Stock> &ret_ptr = stocks_[key];
        if( !ret_ptr ){
            ret_ptr.reset(new Stock(key));
            std::cout << stocks_.size() << " no key: " << key << std::endl;
        }
        return ret_ptr;
    }
private:
//    mutable lmp_tool::MutexLock mutex_;
    std::map<std::string, std::shared_ptr<Stock>> stocks_;

};

}

namespace version2
{

class StockFactory : lmp_tool::noncopyable
{
public:
    StockFactory() {}
    std::shared_ptr<Stock> get(const std::string &key)
    {
//        lmp_tool::MutexLockGuard lock(mutex_);
        std::shared_ptr<Stock> sp_ptr;
        std::weak_ptr<Stock>   &wkptr = stocks_[key];
        sp_ptr = wkptr.lock();
        if( !sp_ptr ){
            sp_ptr.reset(new Stock(key));
            wkptr = sp_ptr;
            std::cout << stocks_.size() << " no key: " << key << std::endl;
        }
        return sp_ptr;
    }
private:
//    mutable lmp_tool::MutexLock mutex_;
    std::map<std::string, std::weak_ptr<Stock>> stocks_;

};

}


namespace version3
{

class StockFactory : lmp_tool::noncopyable
{
public:
    StockFactory() {}
    std::shared_ptr<Stock> get(const std::string &key)
    {
//        lmp_tool::MutexLockGuard lock(mutex_);
        std::shared_ptr<Stock> sp_ptr;
        std::weak_ptr<Stock>   &wkptr = stocks_[key];
        sp_ptr = wkptr.lock();
        if( !sp_ptr ){
            sp_ptr.reset(new Stock(key), std::bind(&StockFactory::delete_stock, this, std::placeholders::_1));
            wkptr = sp_ptr;
            std::cout << stocks_.size() << " no key: " << key << std::endl;
        }
        return sp_ptr;
    }

private:
    void delete_stock(Stock *sp)
    {
        if(sp){
//        lmp_tool::MutexLockGuard lock(mutex_);
            stocks_.erase(sp->key());
        }
    }
private:
//    mutable lmp_tool::MutexLock mutex_;
    std::map<std::string, std::weak_ptr<Stock>> stocks_;

};

}


namespace version4
{

class StockFactory : public lmp_tool::noncopyable,
        std::enable_shared_from_this<StockFactory>
{
public:
    StockFactory() {}
    std::shared_ptr<Stock> get(const std::string &key)
    {
//        lmp_tool::MutexLockGuard lock(mutex_);
        std::shared_ptr<Stock> sp_ptr;
        std::weak_ptr<Stock>   &wkptr = stocks_[key];
        sp_ptr = wkptr.lock();
        if( !sp_ptr ){
            sp_ptr.reset(new Stock(key),
                         std::bind(&StockFactory::delete_stock,
                                   shared_from_this(),
                                   std::placeholders::_1));
            wkptr = sp_ptr;
            std::cout << stocks_.size() << " no key: " << key << std::endl;
        }
        return sp_ptr;
    }

private:
    void delete_stock(Stock *sp)
    {
        if(sp){
//        lmp_tool::MutexLockGuard lock(mutex_);
            stocks_.erase(sp->key());
        }
        delete sp;
    }
private:
//    mutable lmp_tool::MutexLock mutex_;
    std::map<std::string, std::weak_ptr<Stock>> stocks_;

};

}



class StockFactory : public std::enable_shared_from_this<StockFactory>,
                     lmp_tool::noncopyable
{
 public:
  std::shared_ptr<Stock> get(const std::string &key)
  {
    std::shared_ptr<Stock> pStock;
//    muduo::MutexLockGuard lock(mutex_);
    std::weak_ptr<Stock>& wkStock = stocks_[key];
    pStock = wkStock.lock();
    if (!pStock)
    {
      pStock.reset(new Stock(key),
                   std::bind(&StockFactory::weakDeleteCallback,
                               std::weak_ptr<StockFactory>(shared_from_this()),
                               std::placeholders::_1));
      wkStock = pStock;
      std::cout << stocks_.size() << " no key: " << key << std::endl;
    }
    return pStock;
  }

 private:
  static void weakDeleteCallback(const std::weak_ptr<StockFactory>& wkFactory,
                                 Stock* stock)
  {
    printf("weakDeleteStock[%p]\n", stock);
    std::shared_ptr<StockFactory> factory(wkFactory.lock());
    if (factory)
    {
      factory->removeStock(stock);
    }
    else
    {
      printf("factory died.\n");
    }
    delete stock;  // sorry, I lied
  }

  void removeStock(Stock* stock)
  {
    if (stock)
    {
//      lmp_tool::MutexLockGuard lock(mutex_);
      auto it = stocks_.find(stock->key());
//      assert(it != stocks_.end());
      if (it->second.expired())
      {
        stocks_.erase(stock->key());
      }
    }
  }

 private:
//  mutable lmp_tool::MutexLock mutex_;
  std::map<std::string, std::weak_ptr<Stock> > stocks_;
};

void testLongLifeFactory()
{
  std::shared_ptr<StockFactory> factory(new StockFactory);
  {
    std::shared_ptr<Stock> stock = factory->get("NYSE:IBM");
    std::shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
//    assert(stock == stock2);
    // stock destructs here
  }
  // factory destructs here
}

void testShortLifeFactory()
{
  std::shared_ptr<Stock> stock;
  {
    std::shared_ptr<StockFactory> factory(new StockFactory);
    stock = factory->get("NYSE:IBM");
    std::shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
//    assert(stock == stock2);
    // factory destructs here
  }
  // stock destructs here
}

int main()
{
    testLongLifeFactory();
    testShortLifeFactory();
}








