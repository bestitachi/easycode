//出发点,,需要进行域名的后缀匹配.写的一个后缀树,但是粒度比较大,使用了taf的相关组件,读写锁等TODO,后面去掉
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

struct StoreNode //树的节点
{
    string key_;                                          //当前节点的key
    unordered_map<string, shared_ptr<StoreNode>> childs_; //孩子节点集
};
typedef shared_ptr<StoreNode> NodePtr;
class StoreRoot
{
public:
    void Insert(vector<string> &dom)
    {
        TC_ThreadWLock locker(lock_);//加写锁
        NodePtr node_ptr;
        auto it = dom.rbegin();
        auto tree_iter = root_.find(*it);
        //找到叶子,插入
        if (tree_iter != root_.end())
        {
            node_ptr = tree_iter->second;
            ++it;
        }
        else
        {
            node_ptr = make_shared<StoreNode>();
            node_ptr->key_ = *it;
            root_.insert(make_pair(*it, node_ptr));
            ++it;
        }

        for (; it != dom.rend(); ++it)
        {
            if (node_ptr->childs_.find(*it) != node_ptr->childs_.end())
            {
                node_ptr = node_ptr->childs_[*it];
            }
            else
            {
                NodePtr nptr(new StoreNode);
                nptr->key_ = *it;
                node_ptr->childs_.insert(make_pair(*it, nptr));
                node_ptr = nptr;
            }
        }
    }

    bool Search(vector<string> &dom)
    {
        TC_ThreadRLock locker(lock_);//加读锁
        vector<string> hit;
        if(dom.empty())
        {
            return false;
        }
        auto it = dom.rbegin();
        NodePtr node_ptr;
        auto tree_iter = root_.find(*it);
        //找到
        if (tree_iter != root_.end())
        {
            node_ptr = tree_iter->second;
            hit.push_back(*it);
            ++it;
        }
        else
        {
            return false;
        }

        for (; it != dom.rend(); ++it)
        {
            if (node_ptr->childs_.empty()) //查到了叶子
            {
                dom.swap(hit);
                return true;
            }
            auto fiter = node_ptr->childs_.find(*it);
            if (fiter != node_ptr->childs_.end())
            {
                node_ptr = fiter->second;
                hit.push_back(*it);                
            }
            else
            {
                return false;
            }
        }
        if (node_ptr->childs_.empty()) //查到了叶子
        {
            dom.swap(hit);
            return true;
        }
        return false;
    }
private:
    TC_ThreadRWLocker lock_;
    unordered_map<string, NodePtr> root_; //孩子节点集
};
class WhiteDomainTree : public TC_Singleton<WhiteDomainTree>
{
public:
    void Add(const string &domain)
    {
        vector<string> dom = TC_Common::sepstr<string>(domain, ".");
        domain_tree_.Insert(dom);
    }
    bool Search(const string &domain,string & hit)
    {
        vector<string> dom = TC_Common::sepstr<string>(domain, ".");
        if(domain_tree_.Search(dom))
        {
            for(auto it = dom.rbegin();it != dom.rend();++it)
            {
                hit += *it + ".";
            }
            hit.pop_back();
            return true;
        }
        return false;
    }
private:
    StoreRoot domain_tree_;
};
