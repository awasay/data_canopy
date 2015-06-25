#include "canopy.hh"

/**
*	START--Temporary functions
*/

pos_int DataCanopy::getNodeValue(pos_int add){
	return nodes.find(add)->second->statistic->num;
}

/**
*	END--Temporary functions
*/






///////////////////////////////

pos_int scan(data* d, pos_int s){
	int x = 0;
	for (pos_int i = 0; i < s; ++i)
	{
		x+=d[i];
	}
	return x/s;
}

DataCanopy::DataCanopy(mdata* m){
	md=m;
	float log_nc = log2(md->num_chun);
	log_num_chun = ceil(log_nc);
}

pos_int DataCanopy::getAddress(pos_int node, pos_int chunk){
	return node<<log_num_chun|chunk;
}

bool DataCanopy::isLevelOne(pos_int x){
	while (((x & 1) == 0) && x > 1) /* While x is even and > 1 */
   		x >>= 1;
 	return (x == 1);
}

error_code DataCanopy::makeCanopy(){

	pos_int canopy_size = pow(2,(md->num_col));
	
	int num = 0;

	//cout<<canopy_size<<endl;
	for (pos_int k = 0; k < md->num_chun; ++k){
		for (pos_int i = 1; i < canopy_size ; ++i){
			
			node* nd = new node();
			nd->identifier = i;
			std::pair<pos_int,node*> to_insert(i,nd);
			nodes.insert(to_insert);
			for(int j = 0 ;j < 32; ++j){

				if(( (i>>j) &1) == 1){
					num++;
					nd->statistic->num=scan(md->chunk_list[0].vectors[j],md->chunk_list[k].size);
				}
			}
		}
	}
	return 1;

}

pos_int DataCanopy::probeCanopy(){
	
	pos_int n=0;
	pos_int canopy_size = pow(2,(md->num_col));

		for (pos_int i = 1; i < canopy_size ; ++i){
			for (pos_int j = 0; j < md->num_chun ; ++j){
				
				unordered_map<pos_int,node*>::const_iterator got = nodes.find(getAddress(i,j));
				cout<<got->second->statistic->num<<endl;
				if(got==nodes.end())
					continue;
				n++;
			}

		}
	assert(n==nodes.size());
	return n;	

}


error_code DataCanopy::initialize(){
	int n = 0;
	int m = 0;
	for (pos_int i = 0; i < md->num_col; ++i){
		for (pos_int j = i; j < md->num_col; ++j){
			for (pos_int k = 0; k < md->num_chun; ++k){
				if(i==j){
					
				/*****/	

					node* nd = new node();
					nd->statistic = new stat();
					nd->identifier = i;

				/*****/

					nd->statistic->num+=scan(md->chunk_list[k].vectors[i],md->chunk_list[k].size);
					std::pair<pos_int,node*> to_insert(getAddress(1<<i,k),nd);
					nodes.insert(to_insert);
					n++;
					continue;
				}
				
				/*****/
				
				pos_int posn_1 = 1<<i;
				pos_int posn_2 = 1<<j;
				pos_int posn = posn_1|posn_2;
				
				node* nd = new node();
				nd->statistic = new stat();
				nd->identifier = posn;
				
				/*****/
				
				nd->statistic->corelation=calculateCorelation(md->chunk_list[k].vectors[i],md->chunk_list[k].size,md->chunk_list[k].vectors[j],md->chunk_list[k].size);
				std::pair<pos_int,node*> to_insert(getAddress(posn,k),nd);
				nodes.insert(to_insert);
				m++;
			}
		}		
	}
	
	cout<<nodes.size()<<",,canopy_size"<<endl;
	return 1;
}

error_code DataCanopy::makeCanopyCacheFriendly(){

	pos_int canopy_size = pow(2,(md->num_col));
	
	int num = 0;
	
	for (pos_int i = 1; i < canopy_size ; ++i){
		
		/*Create a new node*/
		node* nd = new node();
		nd->statistic = new stat();
		nd->identifier = i;

#ifndef COMPOSABLE
		for(pos_int j = 0 ;j < md->num_col; ++j){

			if(( (i>>j) &1) == 1){
				for (pos_int k = 0; k < md->num_chun; ++k){
					num++;
					nd->statistic->num+=scan(md->chunk_list[k].vectors[j],md->chunk_list[k].size);
#ifdef INSERT			
					std::pair<pos_int,node*> to_insert(getAddress(i,k),nd);
					nodes.insert(to_insert);
#endif
				}
			}
		}
		
#else
		if(isLevelOne(i)){

			for(pos_int j = 0 ;j < md->num_col; ++j){

				if(( (i>>j) &1) == 1){
					for (pos_int k = 0; k < md->num_chun; ++k){
						num++;
						nd->statistic->num+=scan(md->chunk_list[k].vectors[j],md->chunk_list[k].size);
	#ifdef INSERT			
						std::pair<pos_int,node*> to_insert(getAddress(i,k),nd);
						nodes.insert(to_insert);
	#endif
					}
				}
			}
		}else{
			for(pos_int j = 0 ;j < md->num_col; ++j){

				if(( (i>>j) &1) == 1){
				//	int temp = i&(~(1<<j)); // All values at lower levels
					int temp =(1<<j); // All values at level 1
					for (pos_int k = 0; k < md->num_chun; ++k){
						num++;
						nd->statistic->num+=getNodeValue(getAddress(temp,k));
	#ifdef INSERT			
						std::pair<pos_int,node*> to_insert(getAddress(i,k),nd);
						nodes.insert(to_insert);
	#endif
					}
				}
			}
		}
#endif
	}
	cout<<nodes.size()<<",,canopy_size"<<endl;
	return 1;
	
}