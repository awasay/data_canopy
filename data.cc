#include "data.hh"

error_code create_column(column* col, pos_int identifier, pos_int size, data* vector){

	col = new column();
	
	col->identifier = identifier;
	col->size = size;
	col->vector = vector;

	return 1;
}

error_code create_chunk(chunk* chun, pos_int identifier, pos_int size, pos_int num_col, data** vectors){

	chun = new chunk();
	
	chun->identifier = identifier;
	chun->size = size;
	chun->num_col = num_col;	
	vectors = vectors;

	return 1;
}

error_code create_mdata(mdata* md, column* column_list, chunk* chunk_list){
	
	md = new mdata();

	md->column_list = column_list;
	md->chunk_list = chunk_list;

	return 1;
}

error_code chunkify(mdata** md, column* column_list, pos_int size, pos_int num_col){

	
	*md = (mdata*)malloc(sizeof(mdata));



	pos_int num_chunk = column_list[0].size/size;
	
	(*md)->num_chun = num_chunk;
	(*md)->num_col=num_col;
	(*md)->chunk_list = (chunk*)malloc(num_chunk*sizeof(chunk));
	(*md)->column_list = column_list;

	for (pos_int i=0; i<num_chunk; ++i ){
		(*md)->chunk_list[i].vectors = (data**)malloc(num_col*sizeof(data*));
		(*md)->chunk_list[i].identifier=i;
		(*md)->chunk_list[i].size=size;
		
		/*For the last chunk, assign the remainder of the size*/
		
		if(i==num_chunk-1)
			(*md)->chunk_list[i].size=(*md)->column_list[0].size - i*size;
		
		(*md)->chunk_list[i].num_col=num_col;

		for (pos_int j=0; j<num_col; ++j){
			(*md)->chunk_list[i].vectors[j]=&column_list[j].vector[i*size];

		}
	}

	return 1;
}

error_code pretty_print_md(mdata* md){

	cout<<"***  ***  ***"<<endl;
	cout<<"Num of Columns: "<<md->num_col<<endl;
	cout<<"Num of Chunks: "<<md->num_chun<<endl;
	
	for (pos_int i = 0; i < md->num_chun; ++i){
		
		cout<<"chunk "<<i<<endl;
		
		for (pos_int j = 0; j < md->num_col; ++j){
			
			cout<<"\t";
			cout<<md->chunk_list[i].vectors[j][0]; 
		}
		cout<<endl;

		for (pos_int j = 0; j < md->num_col; ++j){
			
			cout<<"\t";
			cout<<"."; 
		}
		cout<<endl;
		for (pos_int j = 0; j < md->num_col; ++j){
			
			cout<<"\t";
			cout<<"."; 
		}
		cout<<endl;
		for (pos_int j = 0; j < md->num_col; ++j){
			
			cout<<"\t";
			cout<<"."; 
		}
		cout<<endl;
		for (pos_int j = 0; j < md->num_col; ++j){
			
			cout<<"\t";
			cout<<md->chunk_list[i].vectors[j][md->chunk_list[i].size-1]; 
		}
		cout<<endl;

	}
	return 1;

}



error_code pretty_print_cols(column* col, pos_int num_col){

	
	for (pos_int j = 0; j < num_col; ++j){
		cout<<"\t";
		cout<<col[j].vector[0]; 
	}
	cout<<endl;

	for (pos_int j = 0; j < num_col; ++j){
		
		cout<<"\t";
		cout<<"."; 
	}
	cout<<endl;
	for (pos_int j = 0; j < num_col; ++j){
		
		cout<<"\t";
		cout<<"."; 
	}
	cout<<endl;
	for (pos_int j = 0; j < num_col; ++j){
		
		cout<<"\t";
		cout<<"."; 
	}
	cout<<endl;
	for (pos_int j = 0; j < num_col; ++j){
		
		cout<<"\t";
		cout<<col[j].vector[col[j].size-1]; 
	}
	cout<<endl;
	return 1;
}