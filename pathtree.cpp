//工作中,为了解决获取s2graph查询过程而写的一个树结构.其中使用了tars.有时间会去掉tars中的东西TODO
class QueryHistory:public TC_HandleBase
{
public:
    struct TreeNode;
    typedef  TC_AutoPtr<TreeNode> TreeNodePtr;
    struct TreeNode:public TC_HandleBase{
        string key;//查询的key,这次查询的to,下次查询的from
        map<string,string> data;//本次查询的结果,
        set<TreeNodePtr> leafs;//存放下次查询的结果.

        bool operator == (const TreeNode & righer) const
        {
            return key == righer.key;
        }
        bool operator < (const TreeNode & righer)const
        {
            return key < righer.key;
        }
    };
    
    typedef  set<TreeNodePtr>::iterator LeafsIt;
    typedef  map<string,string>::iterator DataIt;
    QueryHistory & operator = (const QueryHistory & righer)
    {
        root = righer.root;
        from_nodeptr_ = righer.from_nodeptr_;
        return *this;
    }
    void InitRoot(const string &key)//生成根
    {
        root.key = "0_"+key;
        TreeNodePtr ptr = new TreeNode;
        ptr->key = root.key;
        from_nodeptr_[root.key] = ptr;
        TLOGDEBUG(__FUNCTION__<<"|key = "<<root.key<<endl);
    }
    //将查询结果挂到树,上一个节点上.
    void AddNode(map<string,string> &result,int step)//
    {
        ostringstream oss;
        if(step == 0)
        {
            oss << step <<"_"<<result["from"];
        }
        else
        {
            oss << step-1 <<"_"<<result["from"];
        }
        
        string  from = oss.str();
        TreeNodePtr ptr = new TreeNode;
        ptr->key = result["to"];
        ptr->data = result;
        if(from_nodeptr_.find(from) == from_nodeptr_.end())
        {
            TLOGDEBUG(__FUNCTION__<<"|not exit from "<<from<<endl);
            from_nodeptr_[from] = ptr;
        }
        else
        {
            from_nodeptr_[from]->leafs.insert(ptr);
            
        }
        oss.str("");
        oss << step << "_"<<result["to"];
        from_nodeptr_[oss.str()] = ptr;
        if(from == root.key)
        {
            root.leafs.insert(ptr);
        }
/*        TLOGDEBUG("from = "<<from << "|to = "<<ptr->key<<"|leafs size = "<<from_nodeptr_[from]->leafs.size()<<endl);
        TLOGDEBUG(__FUNCTION__<<"|cur size = "<<ptr->leafs.size()<<"|from = "<<from<<"|to = "<<oss.str()<<endl);*/
    }
    //根节点的data是空的,只有leafs有值,按照深度遍历.拼出路径
    void GetQueryHistory(vector<map<string,string> > &results)
    {	
		MONITOR_REPORT("GetQueryHistory",1);
        for(LeafsIt it = root.leafs.begin();it != root.leafs.end();++it)
        {
            map<string,string> item;
            Get(*it,results);
            //results.push_back(item);
        }
        TLOGDEBUG("GetQueryHistory |" <<results.size()<<endl);
        Sort(results);
    }

    void GetQueryHistory(vector<map<string,string> > &results,unsigned limit)
    {
        vector<map<string,string> > tmp_results;
        // for(LeafsIt it = root.leafs.begin();it != root.leafs.end() && results.size() < limit;++it)
        // {
        //     int deep_length = 1;
        //     map<string,string> item;
        //     Get(*it,item);
        //     results.push_back(item);
        //     //TLOGWARN("GetQueryHistory:"<<results.size()<<endl);
        // }
        GetQueryHistory(tmp_results);

        for(size_t i = 0;i < tmp_results.size() && i < limit;++i)
        {
            results.push_back(move(tmp_results[i]));
        }
    }

    void Get(const TreeNodePtr &ptr,vector<map<string,string> > &results,int deep_length = 1)
    {
        map<string,string> item;
        //TLOGDEBUG(ptr->key<<endl);
        for(DataIt it = ptr->data.begin();it != ptr->data.end();++it)
        {
            if(it->first == "label")
            {
                continue;
            }
            string key= ptr->data["label"]+"_"+it->first;
            item[key] = it->second;
            //TLOGWARN(__FUNCTION__<<"|"<<key<<"= "<<item[key]<<endl);
        }

        if(!ptr->leafs.empty())
        {
            
            
            for(LeafsIt ite = ptr->leafs.begin();ite != ptr->leafs.end();++ite)
            {
                vector<map<string,string> > leafs_result;
                Get(*ite,leafs_result,deep_length+1);
                for(size_t k = 0;k < leafs_result.size();++k)
                {
                    leafs_result[k].insert(item.begin(),item.end());
                }
                results.insert(results.end(),leafs_result.begin(),leafs_result.end());            
            }
        }
        else
        {
            item["deep_length"] = TC_Common::tostr<int>(deep_length);
            results.push_back(item);
        }
        //Show(results);
        // TLOGDEBUG(__FUNCTION__<<"|"<<item["deep_length"]<<"|"<<endl);
    }
    void Show(vector<map<string,string> > &results)
    {
        TLOGDEBUG("******************************start"<<endl);
        for(size_t h = 0;h < results.size();++h)
        {
            map<string,string>::iterator it = results[h].begin();
            for(;it!= results[h].end();++it)
            {
                TLOGDEBUG(it->first<<"|"<<it->second<<endl);
            }
        }
        TLOGDEBUG("******************************end"<<endl);
    }
    //根据最终结果筛选出,与结果对应的路径
    struct SortHander
    {
        bool operator() (map<string, string>  left, map<string, string> right) {
            int left_deep = TC_Common::strto<int>(left["deep_length"]);
            int right_deep = TC_Common::strto<int>(right["deep_length"]);
            return left_deep > right_deep;
        }
    };
    void Select(vector<map<string,string> > &results,vector<map<string,string> > &path)
    {
        vector<map<string,string> > all_path;
        GetQueryHistory(all_path);
        //string label;
        for(size_t i = 0;i < results.size();++i)
        {
            string &label = results[i]["label"];
            for(size_t j = 0;j < all_path.size() && !label.empty();++j)
            {
                string key = label + "_to";
                if(all_path[j][key] == results[i]["to"])
                {
                    path.push_back(all_path[j]);
                }
            }
        }
    }

    void Sort(vector<map<string,string> > &results)
    {
        SortHander sorthandl;
        std::sort<vector<map<string,string> >::iterator,SortHander>(results.begin(),results.end(),sorthandl);
    }
    ~QueryHistory()
    {
    }
private:
    TreeNode root;
    //map<string,TreeNode> key_map_set;
    map<string,TreeNodePtr> from_nodeptr_;
    //TC_ThreadRWLocker conn_locker_;
};
