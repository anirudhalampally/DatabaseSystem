// Team YOLO
// Authors : Anirudh Alampally - 201202173
//         : Ashrith Jalagam   - 201202126
// Build a database system
//----------------------------------------
#include <bits/stdc++.h>
#include <utility> 
using namespace std;

int tablecount=0;
typedef struct cop{
    string first;
    string second;
}cop;
string sep;
class DBSystem {
    private:
        map <string,set < string> > vfunc;
        vector < string > comp;
        vector < string > join;
        typedef struct pair{
            string attr;
            string type;
        }pair;
        typedef struct met{
            string name;
            vector < pair > entries;
        }met;
        vector < string > tables;
        vector < met > metadata;
        int pagesize;
        int num_pages;
        string path;
        int change;
        typedef struct page
        {
            int index;
            int recstart;
            int recend;
            char* lin;
            int map;
        }page;
        typedef struct table
        {
            string name;
            int tableid;
            vector < page > pages;
        }table;
        vector < table > database;
        char** main_mem;
        int page_index;
        typedef struct mapping
        {
            int dbid;
            int pageid;
        }mapping;
        vector < mapping > maps;
        int * db_compare_flag;

    public:
        void readConfig(string configFilePath) {
            string path1= configFilePath + "/config.txt";
            ifstream input;
            input.open(path1.c_str());
            string line;
            int flag=0;
            while ( getline( input , line ) ) {
                istringstream buf(line);
                string p;
                buf >> p;
                if(flag==1)
                {
                    flag=0;
                    tables.push_back(p);
                    met met1;
                    met1.name=p;
                    while(getline(input,line))
                    {
                        istringstream buf(line);
                        getline(buf,p,',');
                        if(p=="END")
                            break;
                        pair pa;
                        pa.attr=p;
                        getline(buf,p,',');
                        pa.type=p;
                        met1.entries.push_back(pa);
                    }
                    metadata.push_back(met1);
                    continue;
                }
                if(p=="BEGIN")
                {
                    flag=1;
                    continue;
                }
                if(p=="PAGE_SIZE")
                {
                    string pp;
                    buf >> p;
                    pp=p;
                    pagesize=atoi(pp.c_str());
                    //cout<<pagesize<<endl;
                }
                if(p=="NUM_PAGES")
                {
                    string pp;
                    buf >> p;
                    pp=p;
                    num_pages=atoi(pp.c_str());
                    //cout<<num_pages<<"kya"<<endl;
                }
                if(p=="PATH_FOR_DATA")
                {
                    string pp;
                    buf >> p;
                    pp=p;
                    path=pp;
                    //cout<<path<<endl;
                }
            }
            db_compare_flag = (int *)malloc(sizeof(int)*num_pages);
            for(int i=0;i<num_pages;i++)
            {
                db_compare_flag[i] = 0;
            } 
            input.close();
        }
        void populateDBInfo() {
            comp.push_back("=");
            comp.push_back(">=");
            comp.push_back("<=");
            comp.push_back("!=");
            comp.push_back(">");
            comp.push_back("<");
            comp.push_back("LIKE");
            change=0;
            main_mem=(char **)malloc(num_pages*sizeof(char *));
            for(int i=0;i<num_pages;i++)
            {
                main_mem[i]=(char *)malloc(pagesize*sizeof(char));
            }
            page_index=0;
            maps.resize(num_pages);
            sep="|";
            for(int i=0;i<tables.size();i++)
            {
                //cout<<tables[i]<<endl;
                table temp1;
                temp1.name=tables[i];
                temp1.tableid=tablecount;
                tablecount++;
                database.push_back(temp1);
                string path_for_csv= path + tables[i] + ".csv";
                //cout<<path_for_csv;
                ifstream opfile;
                int reccount=0;
                opfile.open(path_for_csv.c_str());
                string line;
                int flag=0;
                page pageins;
                //				pageins.lin[0] = '\0';
                pageins.index = 0;
                while ( getline( opfile , line ) ) 
                {
                    //					cout << pagesize - pageins.index << "," << strlen(line.c_str()) << ",Lin -> " << pageins.lin<<endl;					
                    int n=strlen(line.c_str());
                    int adj=0;
                    for(int l=0;l<n;l++)
                    {
                        if(line.c_str()[l]=='\"')
                        {
                            //cout<<"lol";
                            adj++;
                        }
                    }
                    if(pagesize - pageins.index  <= n - adj )
                    {
                        pageins.recend = reccount;
                        pageins.map=-1;
                        database[i].pages.push_back(pageins);
                        flag = 0;	
                    }
                    if(flag==0)
                    {
                        //cout << "lol" << endl;
                        pageins.index=0;
                        pageins.recstart=reccount;
                        pageins.lin=(char *)malloc(sizeof(char)*pagesize);
                        int n=strlen(line.c_str());
                        for(int i=0;i<n;i++)
                        {
                            if(line.c_str()[i]!='\"')
                            {
                                //	cout<<line.c_str()[i];
                                pageins.lin[pageins.index]=line.c_str()[i];
                                pageins.index++;
                            }
                        }
                        pageins.lin[pageins.index++]=sep.c_str()[0];
                        pageins.lin[pageins.index]='\0';
                        flag=1;
                        reccount++;
                        pageins.recend = reccount;
                        continue;
                    }
                    reccount++;
                    pageins.recend = reccount;
                    for(int i=0;i<n;i++)
                    {
                        if(line.c_str()[i]!='\"')
                        {
                            pageins.lin[pageins.index]=line.c_str()[i];
                            pageins.index++;
                        }
                    }
                    pageins.lin[pageins.index++]=sep.c_str()[0];
                    pageins.lin[pageins.index]='\0';
                    //cout<<line<<endl;
                }
                pageins.map=-1;
                database[i].pages.push_back(pageins);
                opfile.close();
            }
            for(int i=0;i<database.size();i++)
            {
                for(int j=0;j<database[i].pages.size();j++)
                {
                    for(int l=0;l<metadata[i].entries.size();l++)
                    {
                        string le=database[i].name;
                        le+=metadata[i].entries[l].attr;
                        for(int k=database[i].pages[j].recstart;k<database[i].pages[j].recend;k++)
                        {
                            string row;
                            row=getRecord(database[i].name,k);
                            string temp=row;
                            istringstream ss(temp);
                            string token;
                            while(getline(ss,token,'|'))
                            {
                                istringstream ss1(token);
                                string token1;
                                int counter=0;
                                while(getline(ss1,token1,','))
                                {
                                    if(counter==l)
                                    {
                                        vfunc[le].insert(token1);
                                    }
                                    counter++;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
        string getRecord(string tableName, int recordId) {
            for(int i=0;i<database.size();i++)
            {
                if(database[i].name==tableName)
                {
                    for(int j=0;j<database[i].pages.size();j++)
                    {
                        if(database[i].pages[j].recstart<=recordId && database[i].pages[j].recend>recordId)
                        {
                            if(database[i].pages[j].map!=-1)
                            {
                                //cout<<database[i].pages[j].map<<" "<<"HIT\n";
                                return main_mem[database[i].pages[j].map];
                            }
                            else
                            {
                                if(num_pages==page_index)
                                {
                                    //cout<<"lol";
                                    page_index=0;
                                    change=1;
                                }
                                if(change==1)
                                {
                                    database[maps[page_index].dbid].pages[maps[page_index].pageid].map=-1;
                                }
                                int k;
                                for(k=0;k<strlen(database[i].pages[j].lin);k++)
                                {
                                    main_mem[page_index][k]=database[i].pages[j].lin[k];
                                }
                                database[i].pages[j].map=page_index;
                                main_mem[page_index][k]=0;
                                maps[page_index].dbid=i;
                                maps[page_index].pageid=j;
                                page_index++;
                                string r;
                                ostringstream convert;
                                convert << page_index-1;
                                r=convert.str();
                                return main_mem[page_index-1];
                                //cout<< "MISS "+r +"\n";

                            }
                        }
                    }
                    break;
                }

            }
            return "record\n";

        }
        void insertRecord(string tableName, string record) {
            for(int i=0;i<database.size();i++)
            {
                if(tableName==database[i].name)
                {
                    for(int j=0;j<metadata[i].entries.size();j++)
                    {
                        string le=tableName;
                        string record1=record;
                        le+=metadata[i].entries[j].attr;
                        istringstream ss(record1);
                        string token;
                        int counter=0;
                        while(getline(ss,token,','))
                        {
                            if(counter==j)
                            {
                                vfunc[le].insert(token);
                            }
                            counter++;
                        }
                    }
                }
            }
            int mainmem_index = 0;
            int db_id=0;
            int flg = 0;
            int temp = 0;
            int lastpage = 0;
            for(int i=0;i<database.size();i++)
            {
                if(database[i].name == tableName)
                {
                    db_id = i;
                    lastpage = database[i].pages.size();
                    //cout<<lastpage<<endl;
                    //Main Memory Checking
                    for(int j=0; j<num_pages; j++)
                    {
                        //Check if last page is in main memory
                        //cout << db_id << endl;
                        //cout << lastpage << endl;
                        //cout << maps[j].pageid;
                        if(maps[j].dbid == db_id && maps[j].pageid == lastpage-1)
                        {
                            mainmem_index = maps[j].pageid;
                            flg = 1;
                            temp = database[i].pages[lastpage-1].map;
                            break;
                        }
                        else
                            flg = 2;
                    }
                }
            }
            //cout<<flg<<"flag:"<<endl;
            //Last page of table in the main memory present
            if(flg == 1)
            {
                //Check if enough space present in the last page
                //cout << database[db_id].pages[mainmem_index].lin << endl;
                //cout << strlen(database[db_id].pages[mainmem_index].lin) << endl;
                int reclen = strlen(record.c_str());
                //Enough place present in the lastpage
                //cout << mainmem_index-1;
                //				cout << main_mem[temp];
                //cout<<main_mem[temp]<<endl;
                if(reclen + strlen(main_mem[temp]) <= pagesize)
                {
                    int lenn = strlen(main_mem[temp]);
                    for(int j=0;j < reclen ; j++)
                    {
                        main_mem[temp][lenn++] = record[j];
                    }
                    main_mem[temp][lenn] = sep.c_str()[0];
                    lenn++;
                    main_mem[temp][lenn] = '\0';
                    //cout << main_mem[temp] << endl;
                    //maps[temp].dbid=db_id;
                    //maps[temp].pageid=lastpage-1;
                    //database[db_id].pages[lastpage-1].map=temp;
                }
                else
                {
                    flg = 3;
                }
            }
            //Not in Main_Memory
            if(flg == 2)
            {
                if(num_pages == page_index)
                {
                    page_index = 0;
                }
                //cout<<"pi"<<page_index<<endl;
                main_mem[page_index][0]='\0';
                //database[db_id].pages[lastpage-1].map=-1;
                int k=0;
                for(k=0; k < strlen(database[db_id].pages[lastpage-1].lin); k++)
                {
                    main_mem[page_index][k] = database[db_id].pages[lastpage-1].lin[k];
                }
                main_mem[page_index][k] = '\0';
                int reclen = strlen(record.c_str());
                //cout << mainmem_index-1<<endl;
                //		cout << main_mem[page_index]<<endl;
                if(reclen + strlen(main_mem[page_index]) <= pagesize)
                {
                    int lenn = strlen(main_mem[page_index]);
                    for(int j=0;j < reclen ; j++)
                    {
                        main_mem[page_index][lenn++] = record[j];
                    }
                    main_mem[page_index][lenn] = sep.c_str()[0];
                    lenn++;
                    main_mem[page_index][lenn] = '\0';
                    //cout << main_mem[page_index] << endl;
                    maps[page_index].dbid=db_id;
                    maps[page_index].pageid=lastpage-1;
                    database[db_id].pages[lastpage-1].map=page_index;
                    //	cout<<lastpage<<endl;
                    //cout<<"dd"<<maps[page_index].pageid<<endl;
                    //cout << main_mem[page_index];
                    page_index++;
                }
                else
                {
                    flg = 3;
                }
            }
            //			cout<<flg<<endl;
            //Not enough place for appending this record , hence creation of a new page
            if(flg == 3)
            {
                int newrecend=0;
                if(num_pages == page_index)
                    page_index = 0;

                main_mem[page_index][0]='\0';
                database[maps[page_index].dbid].pages[maps[page_index].pageid].map=-1;
                newrecend = database[db_id].pages[lastpage-1].recend;
                page newpage;
                newpage.recstart = newrecend;
                newpage.recend = newrecend + 1;
                newpage.lin = (char *)malloc(sizeof(char)*pagesize);
                newpage.lin[0] = '\0';
                newpage.map = page_index;
                int reclen = strlen(record.c_str());
                int j=0;
                for(j=0;j < reclen ; j++)
                {
                    main_mem[page_index][j] = record[j];
                }
                main_mem[page_index][j] = sep.c_str()[0];
                main_mem[page_index][j+1] = '\0';
                //cout<<"-->"<<main_mem[page_index]<<endl;
                //cout<<page_index<<endl;
                maps[page_index].dbid = db_id;
                maps[page_index].pageid = lastpage;
                database[db_id].pages.push_back(newpage);
                page_index++;
                //cout << main_mem[page_index-1] << endl; 
            }
            //cout << flg;
            return;
        }
        void flushPages()
        {
            for(int i=0;i<num_pages;i++)
            {
                //cout<<"lolsdjbajsdbjsabdjb";
                int db=maps[i].dbid;
                int page=maps[i].pageid;
                //cout<<page<<db<<endl;
                if(page==0 && db==0)
                    continue;
                if(strlen(database[db].pages[page].lin)!=strlen(main_mem[i]) && strlen(database[db].pages[page].lin)!=0)
                {
                    database[db].pages[page].recend++;
                }
                database[db].pages[page].lin=main_mem[i];
                //break;
                //cout<<database[db].pages[page].lin<<endl;
            }

            for(int i=0;i<database.size();i++)
            {
                string p;
                p= path + database[i].name + ".csv";
                ofstream output;
                output.open(p.c_str());
                for(int j=0;j<database[i].pages.size();j++)
                {
                    string buf = database[i].pages[j].lin; 
                    //cout<<buf<<endl;;
                    istringstream sss(buf); 
                    string word;
                    while(getline(sss , word , '|')) { 
                        if (word!="") 
                        {
                            output << word; 
                            output << '\n'; 
                        } 
                        //cout << word << endl; 
                    } 

                }
                output.close();	
            }

            return;
        }
        void print(){
            for(int i=0;i<database.size();i++)
            {
                for(int j=0;j<database[i].pages.size();j++)
                {
                    cout<<database[i].pages[j].lin<<endl;
                    cout<<database[i].pages[j].recstart<<endl;
                    cout<<database[i].pages[j].recend<<endl;
                    cout<<database[i].pages[j].map<<endl;
                }
            }
        }

        void queryType(string query) {
            vector < string > words;
            int flag=0;
            istringstream iss(query);
            do{
                string sub;
                iss>>sub;
                words.push_back(sub);
            }while(iss);
            for(int i=0;i<words.size();i++)
            {
                if(words[i][0]=='V')
                {
                    string qq=words[i];
                    qq = qq.substr(2, qq.size()-4);
//                    cout<<qq<<endl;
                    istringstream ss1(qq);
                    string tab,col;
                    getline(ss1,tab,',');
                    getline(ss1,col,',');
                    cout<<V(tab,col)<<endl;
                }
                if(words[i]=="select")
                {
                    flag=1;
                }
                if(words[i]=="insert")
                {
                    flag=2;
                }
                if(words[i]=="ON")
                {
                    flag=3;
                }
            }
            if(flag==1)
            {
                selectCommand(query);
            }
            else if(flag==2){
                createCommand(query);
            }
            else if(flag==3)
            {
                joinCommand(query);
            }
            //cout<<query;
        }	       
        void joinCommand(string query)
        {
            vector < string > tablenames;
            cop condition;
            cop left,right;
            string check;
            vector< cop > columns;
            istringstream iss(query);
            vector < string > words;
            do{
                string sub;
                iss>>sub;
                words.push_back(sub);
            }while(iss);
            int flag=0;
            for(int i=0;i<words.size();i++)
            {
                if(words[i]=="SELECT")
                {
                    flag=1;
                    continue;
                }
                if(words[i]=="FROM")
                {
                    flag=2;
                    continue;
                }
                if(words[i]=="ON")
                {
                    flag=3;
                    continue;
                }
                if(flag==1)
                {
                    string lll=words[i];
                    istringstream ss(lll);
                    string token;
                    vector < string > ll;
                    while(getline(ss,token,'.'))
                    {
                        ll.push_back(token);
                    }
                    cop tt;
                    if(ll[1][ll[1].size()-1]==',')
                        ll[1] = ll[1].substr(0, ll[1].size()-1);
                    tt.first=ll[0];
                    tt.second=ll[1];
                    columns.push_back(tt);
                }
                if(flag==2)
                {
                    if(words[i]!="INNER" && words[i]!="JOIN")
                    {
                        tablenames.push_back(words[i]);
                    }
                }
                if(flag==3)
                {
                    condition.first=words[i];
                    if(words[i+2][words[i+2].size()-1]==';')
                        words[i+2] = words[i+2].substr(0, words[i+2].size()-1);
                    condition.second=words[i+2];
                    check=words[i+1];
                    i+=2;
                    istringstream ss6(condition.first);
                    string token7;
                    getline(ss6,token7,'.');
                    left.first=token7;
                    getline(ss6,token7,'.');
                    left.second=token7;
                    istringstream ss7(condition.second);
                    getline(ss7,token7,'.');
                    right.first=token7;
                    getline(ss7,token7,'.');
                    right.second=token7;
                    break;
                }
            }
            int ascf=1;


            /*            for(int i=0;i<columns.size();i++)
                          {
                          cout<<columns[i].first<<" "<<columns[i].second<<endl;
                          }
                          for(int i=0;i<tablenames.size();i++)
                          {
                          cout<<tablenames[i]<<endl;
                          }
                          cout<<condition.first<<check<<condition.second;*/
            vector < vector < string > > table1;
            vector < vector < string > > table2;
            int flagi1[10000];
            int flagi2[10000];
            int positions3[10000]={0};
            int positions4[10000]={0};
            for(int i=0;i<10000;i++)
            {
                flagi1[i]=1;
                flagi2[i]=1;
            }
            vector < int > positions1,positions2;
            vector < int > positions5,positions6;
            vector < string > orderby_type1,orderby_type2;
            map<string,int> col_to_num1;
            map<string,int> col_to_num2;
            vector < string > orderby1,orderby2;
            orderby1.push_back(left.second);
            orderby2.push_back(right.second);
            for(int i=0;i<database.size();i++)
            {
                for(int j=0;j<tablenames.size();j++)
                {
                    if(database[i].name==tablenames[j])
                    {
                        for(int k=0;k<database[i].pages.size();k++)
                        {
                            for(int l=database[i].pages[k].recstart;l<database[i].pages[k].recend;l++)
                            {
                                string row;
                                row=getRecord(tablenames[j],l);
                                string temp=row;
                                istringstream ss(temp);
                                string token;
                                while(getline(ss,token,'|'))
                                {
                                    // tablevalues.push_back(token);
                                    // cout<<token<<endl;
                                    istringstream ss1(token);
                                    string token1;
                                    vector < string > ro; 
                                    while(getline(ss1,token1,','))
                                    {
                                        ro.push_back(token1);
                                    }
                                    if(j==0)
                                    {
                                        table1.push_back(ro);
                                    }
                                    if(j==1)
                                    {
                                        table2.push_back(ro);
                                    }
                                }
                                break;
                            }
                        }
                        for(int l=0;l<metadata[i].entries.size();l++)
                        {
                            for(int m=0;m<columns.size();m++)
                            {
                                if(columns[m].second==metadata[i].entries[l].attr)
                                {
                                    if(j==0 && database[i].name==columns[m].first)
                                        positions5.push_back(l);
                                    if(j==1 && database[i].name==columns[m].first)
                                        positions6.push_back(l);
                                }
                            }
                            if(j==0 && left.second==metadata[i].entries[l].attr)
                            {
                                positions1.push_back(l);
                                if(orderby1[0]==metadata[i].entries[l].attr)
                                {
                                    string token1 = metadata[i].entries[l].type;
                                    token1=token1.substr(1, token1.size()); 
                                    orderby_type1.push_back(token1);
                                    col_to_num1[metadata[i].entries[l].attr]=l;
                                }

                            }
                            // cout<<right.second<<" "<<metadata[i].entries[l].attr<<endl;
                            if(j==1 && right.second==metadata[i].entries[l].attr)
                            {
                                //cout<<"lol";
                                positions1.push_back(l); 
                                if(orderby2[0]==metadata[i].entries[l].attr)
                                {
                                    string token1 = metadata[i].entries[l].type;
                                    token1=token1.substr(1, token1.size()); 
                                    orderby_type2.push_back(token1);
                                    col_to_num2[metadata[i].entries[l].attr]=l;
                                }
                            }
                        }
                    }
                }
            }
            for(int i=0;i<positions5.size();i++)
            {
            //    cout<<positions5[i]<<endl;
                positions3[positions5[i]]=1;
            }
            for(int i=0;i<positions6.size();i++)
            {
            //    cout<<positions6[i]<<endl;
                positions4[positions6[i]]=1;
            }
            /*for(int i=0;i<tablenames.size();i++)
              {
              for(int j=0;j<columns.size();j++)
              {
              if(columns[j].first==tablenames[i])
              {
              for(int k=0;k<metadata[
              if(i==0)
              {
              positions5.push_back();
              }
              }
              }

              }*/
            /*for(int i=0;i<positions1.size();i++)
              {
              flagi1[positions1[i]]=1;
              }
              for(int i=0;i<positions2.size();i++)
              {
              flagi2[positions2[i]]=1;
              }*/


            /*for(int i=0;i<orderby_type2.size();i++)
              {
              cout<<orderby_type2[i]<<endl;
              }
              for(int i=0;i<orderby_type2.size();i++)
              {
              cout<<orderby_type2[i]<<endl;
              }*/
            string leftjoin=phase_merge(table1,flagi1,orderby1,orderby_type1,col_to_num1,ascf);
            string rightjoin=phase_merge(table2,flagi2,orderby2,orderby_type2,col_to_num2,ascf);
            //cout<<leftjoin<<endl<<rightjoin;
            //string rightjoin;
            vector < vector < string > > l1,r1;
            istringstream ss(leftjoin);
            string token;
            while(getline(ss,token,'\n'))
            {
                istringstream ss1(token);
                string token1;
                vector < string > ro;
                while(getline(ss1,token1,','))
                {
                    ro.push_back(token1);
                }
                l1.push_back(ro);
            }
            istringstream ss2(rightjoin);
            string token2;
            while(getline(ss2,token2,'\n'))
            {
                istringstream ss3(token2);
                string token3;
                vector < string > ro;
                while(getline(ss3,token3,','))
                {
                    ro.push_back(token3);
                }
                r1.push_back(ro);
            }
            int index1,index2;
            for(int i=0;i<database.size();i++)
            {
                for(int j=0;j<tablenames.size();j++)
                {
                    if(tablenames[j]==database[i].name)
                    {
                        for(int k=0;k<metadata[i].entries.size();k++)
                        {
                            if(metadata[i].entries[k].attr==left.second && j==0)
                            {
                                index1=k;
                            }
                            if(metadata[i].entries[k].attr==right.second && j==1)
                            {
                                //cout<<right.second<<" "<<database[i].name<<endl;
                                index2=k;
                            }
                        }
                    }
                }
            }
            //cout<<index1<<" "<<index2<<endl;
            int p1=0,p2=0;
            vector < vector < string > > output;
            int temp=0;
            /*for(int i=0;i<l1.size();i++)
              {
              for(int j=0;j<l1[i].size();j++)
              {
              cout<<l1[i][j]<<" ";
              }
              cout<<endl;
              }
              for(int i=0;i<r1.size();i++)
              {
              for(int j=0;j<r1[i].size();j++)
              {
              cout<<r1[i][j]<<" ";
              }
              cout<<endl;
              }*/
            while(1)
            {
                if(p1>=l1.size() || p2>=r1.size())
                    break;
                if(orderby_type1[0]=="int")
                {
                    if(atoi(l1[p1][index1].c_str())==atoi(r1[p2][index2].c_str()))
                    {
                        //cout<<l1[p1][index1]<<" "<<r1[p2][index2]<<endl;
                        int l2=atoi(l1[p1][index1].c_str());
                        int r2=atoi(r1[p2][index2].c_str());
                        int copo=0,cop;
                        while(p1<l1.size() && l2==atoi(l1[p1][index1].c_str()))
                        {
                            cop=0;
                            while(p2<r1.size() && r2==atoi(r1[p2][index2].c_str()))
                            {
                                vector < string > rowo;
                                rowo=l1[p1];
                                for(int i=0;i<r1[p2].size();i++)
                                {
                                    rowo.push_back(r1[p2][i]);
                                }
                                p2++;
                                cop++;
                                output.push_back(rowo);
                            }
                            p1++;
                            p2-=cop;
                            copo++;
                        }
                        p2+=cop;
                    }
                    else if(p2<r1.size() && atoi(l1[p1][index1].c_str())>atoi(r1[p2][index2].c_str()))
                    {
                        p2++;
                    }
                    else if(p1<l1.size() && atoi(l1[p1][index1].c_str())<atoi(r1[p2][index2].c_str()))
                    {
                        p1++;
                    }
                }
                if(orderby_type1[0]=="String")
                {
                    if(l1[p1][index1]==r1[p2][index2])
                    {
                        //cout<<l1[p1][index1]<<" "<<r1[p2][index2]<<endl;
                        string l2=l1[p1][index1];
                        string r2=r1[p2][index2];
                        int copo=0,cop;
                        while(p1<l1.size() && l2==l1[p1][index1])
                        {
                            cop=0;
                            while(p2<r1.size() && r2==r1[p2][index2])
                            {
                                vector < string > rowo;
                                rowo=l1[p1];
                                for(int i=0;i<r1[p2].size();i++)
                                {
                                    rowo.push_back(r1[p2][i]);
                                }
                                p2++;
                                cop++;
                                output.push_back(rowo);
                            }
                            p1++;
                            p2-=cop;
                            copo++;
                        }
                        p2+=cop;
                    }
                    else if(p2<r1.size() && l1[p1][index1]>r1[p2][index2])
                    {
                        p2++;
                    }
                    else if(p1<l1.size() && l1[p1][index1]<r1[p2][index2])
                    {
                        p1++;
                    }

                }
            }
            for(int i=0;i<output.size();i++)
            {
                for(int j=0;j<output[i].size();j++)
                {
                    if((j<l1[0].size() && positions3[j]==1) || ( j>=l1[0].size() && positions4[j-l1[0].size()]==1)) 
                        cout<<output[i][j]<<" ";
                }
                cout<<endl;
            }



        }
        void selectCommand(string query){
            //cout<<"Querytype:select"<<endl;
            vector < string > words;
            int ascf=1;
            std::size_t found = query.find("(DESC)");
            if (found!=std::string::npos)
            {
                ascf=0;
            }
            istringstream iss(query);
            do{
                string sub;
                iss>>sub;
                words.push_back(sub);
            }while(iss);
            vector < string > tablename;
            vector < string > columns;
            vector < string > columnname;
            vector < string > distinct;
            vector < string > conditions;
            vector < string > orderby;
            vector < string > groupby;
            vector < string > having;
            vector < string > likes;
            vector < string > tablevalues; 
            vector < int > positions;
            vector < string > filtered;
            vector < string > attribute;
            vector < string > condition;
            vector < string > join;
            vector < string > attribute1;
            vector < string > condition1;
            vector < string > join1;
            vector < vector < string > > tabledata;
            vector < string > final;
            int flag=0;
            for(int i=0;i<words.size();i++)
            {
                int flag2=0;
                if(words[i].c_str()[words[i].length()-1]==';')
                {
                    words[i] = words[i].substr(0, words[i].size()-1);
                    flag2=1;
                }
                if(words[i]=="select")
                {
                    flag=1;
                    continue;
                }
                if(words[i]=="from")
                {
                    flag=2;
                    continue;
                }
                if(words[i]=="where")
                {
                    flag=3;
                    continue;
                }
                if(words[i]=="groupby")
                {
                    flag=4;
                    continue;
                }
                if(words[i]=="having")
                {
                    flag=5;
                    continue;
                }
                if(words[i]=="orderby")
                {
                    flag=6;
                    continue;
                }
                if(flag==1)
                {
                    //parse all the columns and store it in a vector
                    /*string temp=words[i];
                      istringstream ss(temp);
                      string token;
                      while(getline(ss,token,','))
                      {
                      columns.push_back(token);
                      }*/
                    columns.push_back(words[i]);
                    //cout<<words[i]<<endl;
                }
                if(flag==2)
                {
                    //parse all the table names and store it in a vector
                    string temp=words[i];
                    istringstream ss(temp);
                    string token;
                    while(getline(ss,token,','))
                    {
                        tablename.push_back(token);
                        //	cout<<token<<endl;
                    }
                    //cout<<words[i]<<endl;
                }
                if(flag==3)
                {
                    conditions.push_back(words[i]);
                    //cout<<words[i]<<endl;
                }
                if(flag==4)
                {
                    //cout<<endl;
                    groupby.push_back(words[i]);
                    //cout<<words[i]<<endl;
                }
                if(flag==5)
                {
                    having.push_back(words[i]);
                    //cout<<words[i]<<" ";
                }
                if(flag==6)
                {
                    //cout<<endl;
                    if(words[i][0]!='(')
                        orderby.push_back(words[i]);
                    //cout<<words[i]<<endl;
                }
                //cout<<words[i].c_str()[words[i].length()-1]<<" ";
                if(flag2)
                {
                    break;
                }
            }
            for(int i=0;i<tablename.size();i++)
            {
                int flag1=0;
                for(int j=0;j<metadata.size();j++)
                {
                    if(metadata[j].name==tablename[i])
                    {
                        flag1=1;
                    }
                }
                if(flag1==0)
                {
                    cout<<"Query Invalid"<<endl;
                    return;
                }
                //cout<<tablename[i]<<endl;
            }
            for(int i=0;i<columns.size();i++)
            {
                int flagd=0;
                for(int j=0;j<columns[i].size();j++)
                {
                    if(columns[i].c_str()[j]=='(')
                    {
                        flagd=1;
                    }
                }
                //cout<<flagd<<endl;
                string to;
                if(flagd)
                {
                    //cout<<columns[i]<<endl;
                    string temp=columns[i];
                    istringstream ss(temp);
                    string token;
                    getline(ss,token,')');
                    string ppp=token;
                    istringstream ss4(ppp);
                    string tokendd;
                    getline(ss,tokendd,')');
                    tokendd = tokendd.substr(1, tokendd.size());
                    //cout<<tokendd<<endl;
                    string token1;
                    istringstream ss1(token);
                    getline(ss1,token1,'(');
                    if(token1!="distinct")
                        continue;
                    getline(ss1,token1,'(');
                    string dis = token1;
                    //cout<<dis<<endl;
                    istringstream ss2(token1);
                    string token3;
                    while(getline(ss2,token3,','))
                    {
                        distinct.push_back(token3);
                        columnname.push_back(token3);
                        //cout<<token3<<endl;
                    }
                    istringstream ss5(tokendd);
                    string token5;
                    while(getline(ss5,token5,','))
                    {
                        columnname.push_back(token5);
                    }
                }
                if(!flagd)
                {
                    string temp=columns[i];
                    istringstream ss(temp);
                    string token;
                    while(getline(ss,token,','))
                    {
                        columnname.push_back(token);
                        //cout<<token<<endl;;
                    }
                }
                //cout<<columns[i]<<endl;
            }
            if(columnname[0]=="*")
            {
                columnname.clear();
                for(int i=0;i<tablename.size();i++)
                {
                    for(int j=0;j<metadata.size();j++)
                    {
                        if(tablename[i]==metadata[j].name)
                        {
                            for(int k=0;k<metadata[j].entries.size();k++)
                            {
                                columnname.push_back(metadata[j].entries[k].attr);	
                                //cout<<metadata[j].entries[k].attr<<endl;	
                            }
                        }
                    }
                }
            }
            int invalid=0;
            for(int i=0;i<columnname.size();i++)
            {
                int flag=0;
                for(int j=0;j<tablename.size();j++)
                {
                    for(int k=0;k<metadata.size();k++)
                    {
                        if(tablename[j]==metadata[k].name)
                        {
                            for(int l=0;l<metadata[k].entries.size();l++)
                            {
                                //cout<<columnname[i]<<" "<<metadata[k].entries[l].attr<<endl;
                                if(metadata[k].entries[l].attr==columnname[i])
                                {
                                    //	cout<<columnname[i]<<" "<<metadata[k].entries[l].attr<<endl;
                                    flag=1;
                                }
                            }
                        }
                    }
                }
                if(flag==0)
                {
                    invalid=1;
                    break;
                }
            }
            if(invalid==1)
            {
                cout<<"Query Invalid"<<endl;
                return;
            }
            invalid=0;
            for(int i=0;i<orderby.size();i++)
            {
                int flag=0;
                for(int j=0;j<tablename.size();j++)
                {
                    for(int k=0;k<metadata.size();k++)
                    {
                        if(tablename[j]==metadata[k].name)
                        {
                            for(int l=0;l<metadata[k].entries.size();l++)
                            {
                                //cout<<columnname[i]<<" "<<metadata[k].entries[l].attr<<endl;
                                if(metadata[k].entries[l].attr==orderby[i])
                                {
                                    //	cout<<columnname[i]<<" "<<metadata[k].entries[l].attr<<endl;
                                    flag=1;
                                }
                            }
                        }
                    }
                }
                if(flag==0)
                {
                    invalid=1;
                    break;
                }
            }
            if(invalid==1)
            {
                cout<<"Query Invalid"<<endl;
                return;
            }
            invalid=0;
            for(int i=0;i<groupby.size();i++)
            {
                int flag=0;
                for(int j=0;j<tablename.size();j++)
                {
                    for(int k=0;k<metadata.size();k++)
                    {
                        if(tablename[j]==metadata[k].name)
                        {
                            for(int l=0;l<metadata[k].entries.size();l++)
                            {
                                //cout<<columnname[i]<<" "<<metadata[k].entries[l].attr<<endl;
                                if(metadata[k].entries[l].attr==groupby[i])
                                {
                                    //	cout<<columnname[i]<<" "<<metadata[k].entries[l].attr<<endl;
                                    flag=1;
                                }
                            }
                        }
                    }
                }
                if(flag==0)
                {
                    invalid=1;
                    break;
                }
            }
            if(invalid==1)
            {
                cout<<"Query Invalid"<<endl;
                return;
            }
            int flagop=0;
            /*for(int j=0;j<conditions.size();j++)
              {
              cout<<conditions[j]<<endl;
              }*/
            for(int i=0;i<conditions.size();i+=3)
            {
                attribute.push_back(conditions[i]);
                condition.push_back(conditions[i+1]);
                attribute.push_back(conditions[i+2]);
                if(i+3<conditions.size())
                {
                    join.push_back(conditions[i+3]);
                    i++;
                }

            }
            for(int i=0;i<condition.size();i++)
            {
                flagop=0;
                for(int j=0;j<comp.size();j++)
                {
                    if(condition[i]==comp[j])
                    {
                        flagop=1;
                        break;
                    }
                }
                if(flagop==0)
                {
                    break;
                }
            }
            if(flagop==0 && condition.size()!=0)
            {
                cout<<"Query Invalid"<<endl;
                return ;
            }
            for(int i=0;i<join.size();i++)
            {
                flagop=0;
                if(join[i]=="AND" || join[i]=="OR")
                {
                    flagop=1;
                }
                if(flagop==0)
                {
                    break;
                }
            }
            if(flagop==0 && join.size()!=0)
            {
                cout<<"Query Invalid"<<endl;
                return ;
            }
            flagop=0;
            for(int i=0;i<metadata.size();i++)
            {
                if(tablename[0]==metadata[i].name)
                {
                    for(int j=0;j<attribute.size();j++)
                    {
                        flagop=0;
                        for(int k=0;k<metadata[i].entries.size();k++)
                        {
                            if(metadata[i].entries[k].attr==attribute[j])
                            {
                                flagop=1;
                            }
                        }
                        if(flagop==0 && j%2==0)
                        {
                            cout<<"Query Invalid"<<endl;
                            return ;
                        }
                    }
                }
                if(flagop==0)
                {
                    break;
                }
            }
            for(int i=0;i<metadata.size();i++)
            {
                if(tablename[0]==metadata[i].name)
                {
                    for(int j=0;j<attribute.size();j+=2)
                    {
                        flagop=0;
                        string s1;
                        string s2;
                        string s3;
                        for(int k=0;k<metadata[i].entries.size();k++)
                        {
                            if(metadata[i].entries[k].attr==attribute[j])
                            {
                                s1=metadata[i].entries[k].type;
                            }
                            if(metadata[i].entries[k].attr==attribute[j+1])
                            {
                                s2=metadata[i].entries[k].type;
                            }
                            int inti=0;
                            int dot=0;
                            for(int l=0;l<attribute[j+1].size();l++)
                            {
                                if(attribute[j+1][l]>='0' && attribute[j+1][l]<='9')
                                {
                                    inti=1;
                                }
                                else
                                {
                                    inti=0;
                                    break;
                                }
                                if(attribute[j+1][l]=='.')
                                {
                                    dot=1;
                                }
                            }
                            if(inti==0)
                            {
                                s3=" String";
                            }
                            if(inti==1 && dot==0)
                            {
                                s3=" int";
                            }
                            if(inti==1 && dot==1)
                            {
                                s3=" float";
                            }
                        }	
                        if(s1!=s2 && s1!=s3)
                        {
                            cout<<"Query Invalid"<<endl;
                            return ;
                        }
                    }
                }
            }
            for(int i=0;i<having.size();i+=3)
            {
                attribute1.push_back(having[i]);
                condition1.push_back(having[i+1]);
                attribute1.push_back(having[i+2]);
                if(i+3<having.size())
                {
                    join1.push_back(having[i+3]);
                    i++;
                }

            }
            for(int i=0;i<condition1.size();i++)
            {
                flagop=0;
                for(int j=0;j<comp.size();j++)
                {
                    if(condition1[i]==comp[j])
                    {
                        flagop=1;
                        break;
                    }
                }
                if(flagop==0)
                {
                    break;
                }
            }
            if(flagop==0 && condition1.size()!=0)
            {
                cout<<"Query Invalid"<<endl;
                return ;
            }
            for(int i=0;i<join1.size();i++)
            {
                flagop=0;
                if(join1[i]=="AND" || join1[i]=="OR")
                {
                    flagop=1;
                }
                if(flagop==0)
                {
                    break;
                }
            }
            if(flagop==0 && join1.size()!=0)
            {
                cout<<"Query Invalid"<<endl;
                return ;
            }
            flagop=0;
            for(int i=0;i<metadata.size();i++)
            {
                if(tablename[0]==metadata[i].name)
                {
                    for(int j=0;j<attribute1.size();j++)
                    {
                        flagop=0;
                        for(int k=0;k<metadata[i].entries.size();k++)
                        {
                            if(metadata[i].entries[k].attr==attribute1[j])
                            {
                                flagop=1;
                            }
                        }
                        if(flagop==0 && j%2==0)
                        {
                            cout<<"Query Invalid"<<endl;
                            return ;
                        }
                    }
                }
                if(flagop==0)
                {
                    break;
                }
            }
            for(int i=0;i<metadata.size();i++)
            {
                if(tablename[0]==metadata[i].name)
                {
                    for(int j=0;j<attribute1.size();j+=2)
                    {
                        flagop=0;
                        string s1;
                        string s2;
                        string s3;
                        for(int k=0;k<metadata[i].entries.size();k++)
                        {
                            if(metadata[i].entries[k].attr==attribute1[j])
                            {
                                s1=metadata[i].entries[k].type;
                            }
                            if(metadata[i].entries[k].attr==attribute1[j+1])
                            {
                                s2=metadata[i].entries[k].type;
                            }
                            int inti=0;
                            int dot=0;
                            for(int l=0;l<attribute1[j+1].size();l++)
                            {
                                if(attribute1[j+1][l]>='0' && attribute1[j+1][l]<='9')
                                {
                                    inti=1;
                                }
                                else
                                {
                                    inti=0;
                                    break;
                                }
                                if(attribute1[j+1][l]=='.')
                                {
                                    dot=1;
                                }
                            }
                            if(inti==0)
                            {
                                s3=" String";
                            }
                            if(inti==1 && dot==0)
                            {
                                s3=" int";
                            }
                            if(inti==1 && dot==1)
                            {
                                s3=" float";
                            }
                        }
                        if(s1!=s2 && s1!=s3)
                        {
                            cout<<"Query Invalid"<<endl;
                            return ;
                        }
                    }
                }
            }
            cout<<"Querytype:select"<<endl;
            cout<<"Tablename:";
            for(int i=0;i<tablename.size();i++)
            {
                cout<<tablename[i];
                if(i<tablename.size()-1)
                    cout<<",";
            }
            if(tablename.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"Columns:";
            for(int i=0;i<columnname.size();i++)
            {
                cout<<columnname[i];
                if(i<columnname.size()-1)
                    cout<<",";
            }
            if(columnname.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"Distinct:";
            for(int i=0;i<distinct.size();i++)
            {
                cout<<distinct[i];
                if(i<distinct.size()-1)
                    cout<<",";
            }
            if(distinct.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"Condition:";
            for(int i=0;i<attribute.size();i+=2)
            {
                cout<<attribute[i]<<" "<<condition[i/2]<<" "<<attribute[i+1]<<" ";
                if(i+2<attribute.size())
                {
                    cout<<join[i/2]<<" ";
                }
            }
            if(attribute.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"Orderby:";
            for(int i=0;i<orderby.size();i++)
            {
                cout<<orderby[i];
                if(i<orderby.size()-1)
                    cout<<",";
            }
            if(orderby.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"Groupby:";
            for(int i=0;i<groupby.size();i++)
            {
                cout<<groupby[i];
                if(i<groupby.size()-1)
                    cout<<",";
            }
            if(groupby.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"Having:";
            for(int i=0;i<attribute1.size();i+=2)
            {
                cout<<attribute1[i]<<" "<<condition1[i/2]<<" "<<attribute1[i+1]<<" ";
                if(i+2<attribute1.size())
                {
                    cout<<join1[i/2]<<" ";
                }
            }
            if(attribute1.size()==0)
            {
                cout<<"NA";
            }
            cout<<endl;
            cout<<"--------"<<endl;
            for(int i=0;i<database.size();i++)
            {
                if(tablename[0]==database[i].name)
                {
                    for(int j=0;j<database[i].pages.size();j++)
                    {
                        for(int k=database[i].pages[j].recstart;k<database[i].pages[j].recend;k++)
                        {
                            string row;
                            row=getRecord(tablename[0],k);
                            string temp=row;
                            istringstream ss(temp);
                            string token;
                            while(getline(ss,token,'|'))
                            {
                                tablevalues.push_back(token);
                            }
                            break;
                        }
                    }

                    for(int j=0;j<columnname.size();j++)
                    {
                        for(int k=0;k<metadata[i].entries.size();k++)
                        {
                            if(columnname[j]==metadata[i].entries[k].attr)
                            {
                                positions.push_back(k);
                            }
                        }
                    }
                }
            }
            for(int i=0;i<tablevalues.size();i++)
            {
                vector < string > rows;
                string temp=tablevalues[i];
                istringstream ss(temp);
                string token;
                while(getline(ss,token,','))
                {
                    rows.push_back(token);
                }
                tabledata.push_back(rows);
                for(int j=0;j<positions.size();j++)
                {
                    filtered.push_back(rows[positions[j]]);
                }
            }
            vector < string > result;
            for(int k=0;k<database.size();k++)
            {
                if(tablename[0]==database[k].name)
                {
                    for(int i=0;i<tablevalues.size();i++)
                    {
                        bool check;
                        string type1;
                        for(int j=0;j<attribute.size();j+=2)
                        {
                            check=false;
                            string con;
                            string s3;

                            int pos;
                            for(int l=0;l<metadata[k].entries.size();l++)
                            {
                                if(attribute[j]==metadata[k].entries[l].attr)
                                {
                                    type1=metadata[k].entries[l].type;
                                    int inti=0;
                                    int dot=0;
                                    for(int m=0;m<attribute[j+1].size();m++)
                                    {
                                        if(attribute[j+1][m]>='0' && attribute[j+1][m]<='9')
                                        {
                                            inti=1;
                                        }
                                        else
                                        {
                                            inti=0;
                                            break;
                                        }
                                        if(attribute[j+1][m]=='.')
                                        {
                                            dot=1;
                                        }
                                    }
                                    if(inti==0)
                                    {
                                        s3=" String";
                                    }
                                    if(inti==1 && dot==0)
                                    {
                                        s3=" int";
                                    }
                                    if(inti==1 && dot==1)
                                    {
                                        s3=" float";
                                    }
                                    if(join.size()==0)
                                    {
                                        if(type1==" String")
                                        {
                                            if(condition[j/2]=="=")
                                            {
                                                if(tabledata[i][l]==attribute[j+1])
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                            if(condition[j/2]=="LIKE")
                                            {
                                                for(int lm=0;lm<tabledata[i][l].length();lm++)
                                                {
                                                    if(toupper(tabledata[i][l][lm])==toupper(attribute[j+1][lm]))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(check==true)
                                                {
                                                    final.push_back(tablevalues[i]);
                                                    check=false;
                                                    break;
                                                }
                                            }
                                        }
                                        if(type1==" int")
                                        {
                                            if(condition[j/2]=="=")
                                            {
                                                if(atoi(tabledata[i][l].c_str())==atoi(attribute[j+1].c_str()))
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                            if(condition[j/2]==">=")
                                            {
                                                if(atoi(tabledata[i][l].c_str())>=atoi(attribute[j+1].c_str()))
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                            if(condition[j/2]=="<=")
                                            {
                                                if(atoi(tabledata[i][l].c_str())<=atoi(attribute[j+1].c_str()))
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                            if(condition[j/2]=="!=")
                                            {
                                                if(atoi(tabledata[i][l].c_str())!=atoi(attribute[j+1].c_str()))
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                            if(condition[j/2]==">")
                                            {
                                                if(atoi(tabledata[i][l].c_str())>atoi(attribute[j+1].c_str()))
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                            if(condition[j/2]=="<")
                                            {
                                                if(atoi(tabledata[i][l].c_str())<atoi(attribute[j+1].c_str()))
                                                {
                                                    final.push_back(tablevalues[i]);
                                                }
                                            }
                                        }

                                    }
                                    if(join.size()>0)
                                    {
                                        if(join[0]=="AND")
                                        {
                                            if(type1==" String")
                                            {
                                                if(condition[j/2]=="=")
                                                {
                                                    if(tabledata[i][l]==attribute[j+1])
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(condition[j/2]=="LIKE")
                                                {
                                                    for(int lm=0;lm<tabledata[i][l].length();lm++)
                                                    {
                                                        if(toupper(tabledata[i][l][lm])==toupper(attribute[j+1][lm]))
                                                        {
                                                            check=true;
                                                        }
                                                        else
                                                        {
                                                            check=false;				
                                                            break;
                                                        }
                                                    }
                                                    if(check==false)
                                                        break;
                                                }
                                            }
                                            if(type1==" int")
                                            {
                                                if(condition[j/2]=="=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())==atoi(attribute[j+1].c_str()))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(condition[j/2]==">=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())>=atoi(attribute[j+1].c_str()))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(condition[j/2]=="<=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())<=atoi(attribute[j+1].c_str()))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(condition[j/2]=="!=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())!=atoi(attribute[j+1].c_str()))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(condition[j/2]==">")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())>atoi(attribute[j+1].c_str()))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                                if(condition[j/2]=="<")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())<atoi(attribute[j+1].c_str()))
                                                    {
                                                        check=true;
                                                    }
                                                    else
                                                    {
                                                        check=false;				
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        if(join[0]=="OR")
                                        {
                                            if(type1==" String")
                                            {
                                                if(condition[j/2]=="=")
                                                {
                                                    if(tabledata[i][l]==attribute[j+1])
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                    }
                                                    else
                                                        break;
                                                }
                                                if(condition[j/2]=="LIKE")
                                                {
                                                    bool check1=false;
                                                    for(int lm=0;lm<tabledata[i][l].length() && check==false;lm++)
                                                    {
                                                        if(toupper(tabledata[i][l][lm])==toupper(attribute[j+1][lm]))
                                                        {
                                                            check1=true;
                                                        }
                                                        else
                                                        {
                                                            check1=false;				
                                                            break;
                                                        }
                                                    }
                                                    if(check1 == true)
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                        continue;
                                                    }
                                                }
                                            }
                                            if(type1==" int")
                                            {
                                                if(condition[j/2]=="=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())==atoi(attribute[j+1].c_str()))
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                //cout<<condition[j/2]<<endl;
                                                        check=true;
                                                    }
                                                }
                                                if(condition[j/2]==">=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())>=atoi(attribute[j+1].c_str()))
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                        check=true;
                                                    }
                                                }
                                                if(condition[j/2]=="<=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())<=atoi(attribute[j+1].c_str()))
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                        check=true;
                                                    }
                                                }
                                                if(condition[j/2]=="!=")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())!=atoi(attribute[j+1].c_str()))
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                        check=true;
                                                    }
                                                }
                                                if(condition[j/2]==">")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())>atoi(attribute[j+1].c_str()))
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                        check=true;
                                                    }
                                                }
                                                if(condition[j/2]=="<")
                                                {
                                                    if(atoi(tabledata[i][l].c_str())<atoi(attribute[j+1].c_str()))
                                                    {
                                                        final.push_back(tablevalues[i]);
                                                        check=true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if(type1=="OR")
                                continue;
                            if(check==false)
                            {
                                break;
                            }
                        }
                        if(check==true && type1!="OR")
                        {
                            final.push_back(tablevalues[i]);
                        }
                    }
                }
            }
            vector < vector < string > > fin;
            vector < vector < string > > lola;
            for(int i=0;i<final.size();i++)
            {
                vector < string > fina;
                vector < string > fina1;
                vector < string > rows;
                string temp=final[i];
                istringstream ss(temp);
                string token;
                while(getline(ss,token,','))
                {
                    rows.push_back(token);
                }

                for(int j=0;j<positions.size();j++)
                {
                    fina.push_back(rows[positions[j]]);
                }
                for(int j=0;j<rows.size();j++)
                {
                    fina1.push_back(rows[j]);
                }
                lola.push_back(fina1);
                fin.push_back(fina);
            }
            //cout<<orderby[0]<<endl;
            vector < int > ordpos;
            vector < string > orderby_type;
            map<string,int> col_to_num;
            int cc=0;
            for(int i=0;i<database.size();i++)
            {
                if(database[i].name==tablename[0])
                {
                    for(int j=0;j<metadata[i].entries.size();j++)
                    {
                        for(int k=0;k<orderby.size();k++)
                        {
                            if(metadata[i].entries[j].attr==orderby[k])
                            {
                                ordpos.push_back(j);
                                string token1 = metadata[i].entries[j].type; 
                                token1=token1.substr(1, token1.size()); 
                                orderby_type.push_back(token1);
                                col_to_num[metadata[i].entries[j].attr]=j;
                                //cout<<metadata[i].entries[j].type;
                                //cout<<j<<endl;
                            }
                        }
                    }
                }
            }
            sort(positions.begin(),positions.end());
            for(int i=0;i<ordpos.size();i++)
            {
                for(int j=0;j<positions.size();j++)
                {
                    if(ordpos[i]==positions[j])
                    {
                        ordpos[i]=j;
                    }
                }
            }
            if(ordpos.size()==0)
            {
                for(int i=0;i<fin.size();i++)
                {
                    for(int j=0;j<fin[i].size();j++)
                    {
                        cout<<fin[i][j];
                        if(j!=fin[i].size()-1)
                            cout<<",";
                    }
                    cout<<endl;
                }
            }
            int flagi[10000] = {0};
            for(int i=0;i<positions.size();i++)
            {
                flagi[positions[i]]=1;
                //cout<<positions[i]<<endl;
            }

            /*if(ordpos.size()!=0)	
              {
              for(int i=0;i<fin.size()-1;i++)
              {
              for(int j=0;j<fin.size()-i-1;j++)
              {
              if(fin[j][ordpos[0]]  == fin[j+1][ordpos[0]])
              {
              if(fin[j][ordpos[1]]  > fin[j+1][ordpos[1]])
              {
              vector < string > temp = fin[j];
              fin[j] = fin[j+1];
              fin[j+1] = temp;
              }
              }
              if(fin[j][ordpos[0]]  > fin[j+1][ordpos[0]])
              {
              vector < string > temp = fin[j];
              fin[j] = fin[j+1];
              fin[j+1] = temp;
              }
              }
              }
              }*/
            if(ordpos.size()!=0)
                cout<<phase_merge(lola,flagi,orderby,orderby_type,col_to_num,ascf);


        }
        void createCommand(string query){
            //cout<<query<<endl;
            istringstream ss(query);
            string token;
            getline(ss,token,' ');
            getline(ss,token,' ');
            getline(ss,token,' ');
            //cout<<token<<endl;
            int invalid=0;
            for(int i=0;i<metadata.size();i++)
            {
                if(token==metadata[i].name)
                {
                    invalid=1;
                    break;
                }
            }
            if(invalid==1)
            {
                cout<<"Query Invalid"<<endl;
                return ;
            }
            ofstream csvfile ((path+token+".csv").c_str());
            ofstream datafile ((path+token+".data").c_str());
            ofstream confile;
            confile.open((path+"config.txt").c_str(),ios_base::app);
            string token1;
            getline(ss,token1,'(');
            getline(ss,token1,'(');
            token1 = token1.substr(0, token1.size()-1);
            string temp1=token1;
            istringstream ss1(temp1);
            string token2;
            invalid=0;
            while(getline(ss1,token2,','))
            {
                //cout<<token2;
                istringstream ss3(token2);
                string token4;
                getline(ss3,token4,' ');
                getline(ss3,token4,' ');
                string in="int";
                string st="string";
                string bi="binary";
                string fl="float";
                string da="date";
                if(token4==in || token4==st || token4==bi || token4==fl || token4==da)
                {
                    invalid=0;
                }
                else
                {
                    cout<<token4<<endl;
                    invalid=1;
                }
            }
            if(invalid==1)
            {
                cout<<"Query Invalid"<<endl;
                return ;
            }

            cout<<"Querytype:create"<<endl;
            cout<<"Tablename:"<<token<<endl;
            //token1 = token1.substr(0, token1.size()-1);
            cout<<token1<<endl;
            //datafile << token1;
            confile<<"BEGIN\n";
            confile<<token+"\n";
            istringstream ss4(token1);
            string token5;
            getline(ss4,token5,',');
            met m;
            m.name=token;
            while(1)
            {
                pair p;

                istringstream ss5(token5);
                string token6;
                getline(ss5,token6,' ');
                confile<<token6;
                datafile<<token6;
                p.attr=token6;
                datafile<<":";
                confile<<", ";
                getline(ss5,token6,' ');
                confile<<token6;
                p.type=token6;
                confile<<"\n";
                datafile<<token6;
                if(getline(ss4,token5,','))
                    datafile<<",";
                else
                    break;
                m.entries.push_back(p);
            }
            metadata.push_back(m);
            confile<<"END";
            datafile.close();
            csvfile.close();
            confile.close();

        }
        void printmetadata(){
            for(int i=0;i<metadata.size();i++)
            {
                cout<<metadata[i].name<<endl;
                for(int j=0;j<metadata[i].entries.size();j++)
                {
                    cout<<metadata[i].entries[j].attr<<" ";
                    cout<<metadata[i].entries[j].type<<endl;
                }
            }
        }

        void insert_into_cache(string st,int pos)
        {
            vector < int > tempo;
            main_mem[pos][0] = '\0';
            tempo.push_back(1);
            strcpy(main_mem[pos],st.c_str());
            db_compare_flag[pos] = 0;
        }

        bool rec_compare(vector<string> rec1, vector<string> rec2, vector<string> cols, vector<string> cols_type, map<string,int> col_to_num)
        {
            for(int j=0;j<cols.size();j++)
            {
                string st = "String";
                string fl = "float";
                string in = "int";
                if(cols_type[j]==fl || cols_type[j]==in)
                {
                    int k=j;
                    //cout<<cols_type[j]<<endl;
                    if(atoi(rec1[col_to_num[cols[j]]].substr(0,rec1[col_to_num[cols[j]]].length()).c_str()) > atoi(rec2[col_to_num[cols[j]]].substr(0,rec2[col_to_num[cols[j]]].length()).c_str()))
                        return true;
                    else if(atoi(rec1[col_to_num[cols[j]]].substr(0,rec1[col_to_num[cols[j]]].length()).c_str()) < atoi(rec2[col_to_num[cols[j]]].substr(0,rec2[col_to_num[cols[j]]].length()).c_str()))
                        return false;
                    else
                        return false;
                }
                //cout<<cols_type[j]<<endl;
                if(cols_type[j]==st)
                {
                    int k=j;
                    //                cout<<cols_type[j]<<rec1[col_to_num[cols[j]]]<<endl;
                    if(rec1[col_to_num[cols[j]]] > rec2[col_to_num[cols[j]]])
                        return true;
                    else if(rec1[col_to_num[cols[j]]] < rec2[col_to_num[cols[j]]])
                        return false;
                    else
                        return false;
                }
            }
            return true;
        }


        string phase_merge(vector < vector<string> > records, int flag[], vector <string> cols, vector <string> cols_type, map<string,int> col_to_num,int ascflag)
        {
            int i,j,k,l;
            vector < int > tempo;
            vector < vector<string> > phase1;
            for(i=0;i<records.size();i+=num_pages)
            {
                vector <string> temp;
                tempo.push_back(i);
                if(records.size()-i<num_pages)
                {
                    for(j=i;j<records.size();j++)
                    {
                        tempo.push_back(j);
                        string s = "";
                        for(k=0;k<records[j].size();k++)
                        {
                            tempo.push_back(j);
                            s += records[j][k];
                            s+= ",";
                        }
                        tempo.push_back(j);
                        insert_into_cache(s,j-i);
                    }
                    tempo.push_back(j);
                    for(j=0;j<records.size()-i;j++)
                    {
                        int small = smallest_in_db(0,records.size()-i,cols,cols_type,col_to_num);
                        tempo.push_back(j);
                        db_compare_flag[small] = 1;
                        string to_in(main_mem[small]);
                        temp.push_back(to_in);
                        tempo.push_back(j);
                    }
                }
                else
                {
                    tempo.push_back(i);
                    for(j=i;j<i+num_pages;j++)
                    {
                        tempo.push_back(j);
                        string s ="";
                        for(k=0;k<records[j].size();k++)
                        {
                            tempo.push_back(j);
                            s += records[j][k];
                            s += ",";
                            tempo.push_back(j);
                        }
                        insert_into_cache(s,j-i);
                    }
                    for(j=0;j<num_pages;j++)
                    {
                        tempo.push_back(j);
                        int small = smallest_in_db(0,num_pages,cols,cols_type,col_to_num);
                        db_compare_flag[small] = 1;
                        tempo.push_back(small);
                        string to_in(main_mem[small]);
                        temp.push_back(to_in);
                        tempo.push_back(small);
                    }
                }
                phase1.push_back(temp);
            }
            tempo.push_back(i);
            vector < vector<string> > phase2 = phase1;

            tempo.push_back(i);
            while(phase2.size() != 1)
            {
                tempo.push_back(i);
                vector < vector<string> > main_temp;
                for(i=0;i<phase2.size();i+=num_pages)
                {
                    tempo.push_back(i);
                    vector <string> temp;
                    int store_count[100000] = {0};
                    if(phase2.size()-i<num_pages)
                    {
                        tempo.push_back(i);
                        for(j=i;j<phase2.size();j++)
                        {
                            insert_into_cache(phase2[j][store_count[j-i]],j-i);
                        }
                        int breakflag =0;
                        tempo.push_back(i);
                        int counter = 0;
                        while(1)
                        {
                            int small = smallest_in_db(0,phase2.size()-i,cols,cols_type,col_to_num);
                            tempo.push_back(i);
                            string to_in(main_mem[small]);
                            temp.push_back(to_in);
                            store_count[small]++;
                            tempo.push_back(i);
                            if(phase2[i+small].size() == store_count[small])
                            {
                                breakflag++;
                                db_compare_flag[small] = 1;
                                tempo.push_back(i);
                            }
                            else
                                insert_into_cache(phase2[i+small][store_count[small]],small);
                            if(breakflag == phase2.size()-i)
                                break;
                        }

                    }
                    else
                    {
                        tempo.push_back(i);
                        for(j=i;j<num_pages+i;j++)
                        {
                            insert_into_cache(phase2[j][store_count[j-i]],j-i);
                            tempo.push_back(i);
                        }
                        int breakflag =0;
                        while(1)
                        {
                            int small = smallest_in_db(0,num_pages,cols,cols_type,col_to_num);
                            tempo.push_back(i);
                            string to_in(main_mem[small]);
                            temp.push_back(to_in);
                            store_count[small]++;
                            tempo.push_back(i);
                            if(phase2[i+small].size() == store_count[small])
                            {
                                breakflag++;
                                tempo.push_back(i);
                                db_compare_flag[small] = 1;
                            }
                            else
                                insert_into_cache(phase2[i+small][store_count[small]],small);
                            if(breakflag == num_pages)
                                break;
                        }
                    }
                    tempo.push_back(i);
                    main_temp.push_back(temp);
                }
                tempo.push_back(i);
                phase2 = main_temp;
            }
            vector <string> ans = phase2[0];
            if(ascflag==1)
                reverse(ans.begin(),ans.end());
            //cout<<ans.size()<<endl;
            string finale;
            for(j=0;j<ans.size();j++)
            {

                /*Tokenizer an;
                  an.set(ans[j]);
                  an.setDelimiter(",");
                  string te;
                  int counter =0;
                  cout << "[ ";
                  while((te=an.next())!="")
                  {
                  if(flag[counter]==1)
                  cout << te << " ";
                  counter++;
                  }
                  cout << "]";
                  cout << endl;*/
                //cout<<ans[j]<<endl;
                string te=ans[j];
                istringstream ss(te);
                string token;
                int counter=0;
                while(getline(ss,token,','))
                {
                    if(flag[counter]==1)
                    {
                        finale+=token;
                        finale+=',';
                    }
                    counter++;
                }
                finale+='\n';
            }
            return finale;
        }

        int smallest_in_db(int start,int end,vector <string> cols, vector <string> cols_type, map<string,int> col_to_num)
        {
            int s = start;
            vector < int > tempi;

            tempi.push_back(1);
            while(db_compare_flag[s]!=0)
                s++;
            for(int i=0;i<end;i++)
            {
                tempi.push_back(i);
                if(db_compare_flag[i]==0)
                {
                    if(db_compare(i,s,cols,cols_type,col_to_num))
                    {
                        tempi.push_back(i);
                        s = i;
                    }
                }
            }
            return s;
            tempi.push_back(2);
        }



        bool db_compare(int n, int m,vector <string> cols, vector <string> cols_type, map<string,int> col_to_num)
        {
            vector <string> rec1;
            vector <string> rec2;
            string str1(main_mem[n]);
            string str2(main_mem[m]);
            /*Tokenizer r1;
              r1.set(str1);
              r1.setDelimiter(",");
              string t1;
              while((t1 = r1.next())!="")
              rec1.push_back(t1);
              Tokenizer r2;
              r2.set(str2);
              r2.setDelimiter(",");
              string t2;
              while((t2 = r2.next())!="")
              rec2.push_back(t2);
             */
            string temp1=str1;
            istringstream ss(temp1);
            string token1;
            while(getline(ss,token1,','))
                rec1.push_back(token1);
            string temp2=str2;
            istringstream ss1(temp2);
            string token2;
            while(getline(ss1,token2,','))
                rec2.push_back(token2);

            //cout<<str1<<endl<<str2<<endl;
            return rec_compare(rec1,rec2,cols,cols_type,col_to_num);
        }
        int V(string tablename,string attributename)
        {
            string le;
            le=tablename+attributename;
            return vfunc[le].size();
        }


};

int main(int argc,char *argv[])
{
    /*DBSystem obj;
      obj.readConfig(".");
      obj.populateDBInfo();
      cout<<obj.getRecord("employee",3)<<endl;
      cout<<obj.getRecord("employee",2)<<endl;
      cout<<obj.getRecord("employee",1)<<endl;
      cout<<obj.getRecord("student",1)<<endl;
      obj.insertRecord("student","a,j1idusbfubdfokjbnsdofb");
    //obj.insertRecord("employee","3,ufb");
    //obj.insertRecord("student","e,fubdfokjbnsdofb");
    obj.flushPages();
    //	obj.print();*/
    if(argc!=3)
    {
        cout<<"Expected number of arguments are 3"<<endl;
        return 0;
    }
    DBSystem dbs;
    dbs.readConfig(argv[1]);
    dbs.populateDBInfo();
    //dbs.insertRecord("countries","1,AN,Ani,AI");
    //dbs.insertRecord("countries","1,AN,Ani,AJ");
    //dbs.insertRecord("state","1,Chelsea,Europe,EE");
    //dbs.flushPages();
    //cout<<dbs.V("countries","CONTINENT")<<endl;
    //cout<<dbs.V("countries","ID")<<endl;
    //cout<<dbs.V("state","ID")<<endl;
    //cout<<dbs.V("state","COUNTRY")<<endl;
    //	dbs.printmetadata();
    /*cout<<dbs.getRecord("countries",0);
      cout<<dbs.getRecord("countries",1);
      cout<<dbs.getRecord("countries",2);
      dbs.insertRecord("countries", "record");
    //	dbs.flushPages();
    cout<<dbs.getRecord("countries",2);
    cout<<dbs.getRecord("countries",2);
    cout<<dbs.getRecord("countries",3);
    cout<<dbs.getRecord("countries",41);
    cout<<dbs.getRecord("countries",9);
    cout<<dbs.getRecord("countries",39);
    cout<<dbs.getRecord("countries",28);
    cout<<dbs.getRecord("countries",1);
    cout<<dbs.getRecord("countries",30);
    cout<<dbs.getRecord("countries",38);
    cout<<dbs.getRecord("countries",39);
    cout<<dbs.getRecord("countries",31);
    dbs.insertRecord("countries", "record");
    //dbs.flushPages();
    cout<<dbs.getRecord("countries",42);
    cout<<dbs.getRecord("countries",28);*/
    //dbs.print();
    //dbs.print();
    //int t;
    //cin>>t;
    ifstream filep(argv[2]);
    if(!filep.is_open())
    {
        cout<<"Could not open file"<<endl;
    }
    else
    {
        string x;
        int co=0;
        while(getline(filep,x))
        {
            cout<<co<<endl;
            //cout<<x<<endl;
            co++;
            dbs.queryType(x);
            cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`"<<endl;
        }
    }
    //char query[1000];
    /*while(t--)
    {
        getchar();
        scanf("%[^\n]", query);
        //cout<<query<<endl;
        //getline(cin,query);
        //string st;
        //st = query.substr(0, query.size()-1);
        dbs.queryType(query);
    }*/
    /*	dbs.queryType("select ID from countries;");
        dbs.queryType("select distinct(ID,CODE),NAME from countries where ID>CODE;");
        dbs.queryType("select * from countries,countries where ID=CODE AND CODE=ID groupby NAME having CODE=50 orderby CONTINENT;");
        dbs.queryType("select CONTINENT,NAME from <non_existing_table>;");
    //	dbs.queryType("create table states (ID int,CODE int)");	
    dbs.queryType("select ID,NAME from countries where CODE!ID groupby ID;");*/

    return 0;
}
