// Team YOLO
// Authors : Anirudh Alampally - 201202173
//         : Ashrith Jalagam   - 201202126
// Build a database system
//----------------------------------------
#include <bits/stdc++.h>
using namespace std;

int tablecount=0;
string sep;
class DBSystem {
	private:
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
					/*string pp;
					  pp=p;
					  stringstream ss(pp);
					  int i;
					  while(ss >> i)
					  {
					  cout<<i<<endl;
					  if(ss.peek()==',')
					  ss.ignore();
					  }*/
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
			input.close();
		}
		void populateDBInfo() {
			change=0;
			//cout<<"lol";
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
							//		adj++;
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
								cout<<database[i].pages[j].map;
								return "HIT\n";
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
								//cout<<main_mem[page_index-1]<<endl;
								return "MISS "+r +"\n";

							}
						}
					}
					break;
				}

			}

			return "record\n";

		}
		void insertRecord(string tableName, string record) {
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
			//Last page of table in the main memory present
			if(flg == 1)
			{
				//Check if enough space present in the last page
				//cout << database[db_id].pages[mainmem_index].lin << endl;
				//cout << strlen(database[db_id].pages[mainmem_index].lin) << endl;
				int reclen = strlen(record.c_str());
				//Enough place present in the lastpage
				//cout << "loll";
				//cout << mainmem_index-1;
				//				cout << main_mem[temp];
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
				}
				else
				{
					flg = 3;
				}
			}
			//cout << flg<<endl;
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
					//cout<<database[i].pages[j].lin<<endl;
					//cout<<database[i].pages[j].recstart<<endl;
					//cout<<database[i].pages[j].recend<<endl;
					//cout<<database[i].pages[j].map<<endl;
					//cout<<database[i].pages[j].index<<endl;
				}
			}
			/*	for(int i=0;i<num_pages;i++)
				{
				cout<<maps[i].dbid<<endl;
				cout<<maps[i].pageid<<endl;

				}*/
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
				//cout<<words[i]<<endl;
				if(words[i]=="select")
				{
					flag=1;
				}
				if(words[i]=="insert")
				{
					flag=2;
				}
			}
			if(flag==1)
			{
				selectCommand(query);
			}
			else{
				createCommand(query);
			}
			//cout<<query;

		}	       
		void selectCommand(string query){
			cout<<"Querytype:select"<<endl;
			vector < string > words;
			istringstream iss(query);
			do{
				string sub;
				iss>>sub;
				words.push_back(sub);
			}while(iss);
			vector < string > tablename;
			vector < string > columns;
			vector < string > distinct;
			vector < string > conditions;
			vector < string > orderby;
			vector < string > groupby;
			vector < string > having;
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
					string temp=words[i];
					istringstream ss(temp);
					string token;
					while(getline(ss,token,','))
					{
						columns.push_back(token);
					}
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
					}
					//cout<<words[i]<<endl;
				}
				if(flag==3)
				{
					conditions.push_back(words[i]);
					//cout<<words[i]<<" ";
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
				if(flagd)
				{
					cout<<columns[i];
					string temp=columns[i];
					istringstream ss(temp);
					string token;
					while(getline(ss,token,'('))
					{
						getline(ss,token,'(');
						string dis = token;
						dis = dis.substr(0, dis.size()-1);	
						cout<<dis<<endl;
						/*string temp1=dis;
						  istringstream ss1(temp1);
						  string token1;
						  while(getline(ss1,token1,','))
						  {
						  cout<<token1<<endl;
						  }*/
					}


				}
				//cout<<columns[i]<<endl;
			}
		}
		void createCommand(string query){
			cout<<"Querytype:create"<<endl;
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


};

int main()
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

	DBSystem dbs;
	dbs.readConfig(".");
	dbs.populateDBInfo();
	/*ifstream infile;
	  infile.open("test.txt");
	  string line;
	  while ( getline( infile , line ) )
	  {
	  istringstream buf(line);
	  string temp;
	  buf >> temp;
	  cout << temp;
	  }*/
	//dbs.print();

	cout<<dbs.getRecord("transactions",13);
	cout<<dbs.getRecord("transactions",16);
	cout<<dbs.getRecord("transactions",27);
	cout<<dbs.getRecord("transactions",25);
	cout<<dbs.getRecord("transactions",23);
	cout<<dbs.getRecord("transactions",25);
	cout<<dbs.getRecord("transactions",16);
	cout<<dbs.getRecord("transactions",12);
	cout<<dbs.getRecord("transactions",9);
	cout<<dbs.getRecord("transactions",1);
	cout<<dbs.getRecord("transactions",2);
	cout<<dbs.getRecord("transactions",7);
	cout<<dbs.getRecord("transactions",20);
	cout<<dbs.getRecord("transactions",19);
	cout<<dbs.getRecord("transactions",23);
	cout<<dbs.getRecord("transactions",16);
	cout<<dbs.getRecord("transactions",0);
	cout<<dbs.getRecord("transactions",6);
	cout<<dbs.getRecord("transactions",22);
	cout<<dbs.getRecord("transactions",16);
	cout<<dbs.getRecord("transactions",11);
	cout<<dbs.getRecord("transactions",8);
	cout<<dbs.getRecord("transactions",27);
	cout<<dbs.getRecord("transactions",9);
	cout<<dbs.getRecord("transactions",2);
	cout<<dbs.getRecord("transactions",20);
	cout<<dbs.getRecord("transactions",2);
	cout<<dbs.getRecord("transactions",13);
	cout<<dbs.getRecord("transactions",7);
	cout<<dbs.getRecord("transactions",25);

	//	dbs.print();
	//	cout<<dbs.getRecord("transactions",13);
	//	cout<<dbs.getRecord("countries",1);
	//	cout<<dbs.getRecord("countries",2);
	//	dbs.insertRecord("countries", "record");
	//	dbs.flushPages();
	//	dbs.print();
	//	cout<<dbs.getRecord("countries",2);
	//	cout<<dbs.getRecord("countries",2);
	//	cout<<dbs.getRecord("countries",3);
	//	cout<<dbs.getRecord("countries",41);
	//	cout<<dbs.getRecord("countries",9);
	//	cout<<dbs.getRecord("countries",39);
	//	cout<<dbs.getRecord("countries",28);
	//	cout<<dbs.getRecord("countries",1);
	//	cout<<dbs.getRecord("countries",30);
	//	cout<<dbs.getRecord("countries",38);
	//	cout<<dbs.getRecord("countries",39);
	//	cout<<dbs.getRecord("countries",31);
	//	dbs.insertRecord("countries", "record");
	//dbs.flushPages();
	//	cout<<dbs.getRecord("countries",42);
	//	cout<<dbs.getRecord("countries",28);
	//dbs.queryType("select <col1>,<col2> from countries;");
	//dbs.queryType("select distinct(<col1>,<col2>),<col3> from countries where <col1> > <value>;");
	//dbs.queryType("select * from countries,countries where <col1> = <col2> groupby <col1> having <col1> = 50 orderby <col2>;");
	//dbs.queryType("select <col1>,<col2> from <non_existing_table>;");

	return 0;
}
