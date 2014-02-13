#include "BigQ.h"

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	
	this->run_no = 0;
	
	
	buffer = new Page[runlen];//set to runlen +1 to use indxing starting from 1
	//runBuff = new Record[pageLength];//how many records per page?
	
 	
	args_phase1.input = &in;
	args_phase1.sort_order = &sortorder;
	args_phase1.run_length = &runlen;
	
	pthread_create (&worker, NULL, TPMMS_phase1 , (void *)&args_phase1);
	
	// read data from in pipe sort them into runlen pages
	
    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe
	out.ShutDown ();
}

BigQ::quicksort(std::vector<Record> &rb, int left, int right){  

	int i = left;
	int j = right;

   Record* pivot = rb.at((left+right)/2);
  
   // partition  
   while (i <= j) {  
       while (compare(rb.at(i),pivot,sortorder)<0)
           i++;  
  
       while (compare(rb.at(j),pivot,sortorder)>0)
           j--;  
  
       if (i <= j) {  
           Record tmp = rb.at(i);  
           rb.at(i) = rb.at(j);  
           rb.at(j) = tmp;  
  
           i++;  
           j--;  
       }  
   }  
  
   // recursion  ?
   if (left < j)  
       quickSort(rb, left, j);  
  
   if (i < right)  
       quickSort(rb, i, right);  
}  



BigQ::void* TPMMS_Phase1(void* arg){
	/*
	Pipe *input;
		OrderMaker *sort_order;
		int run_length;
		Record *temporary = &temp;
		Page *buf = buffer;
		*/
	int page_Index = -1;//same as below indexing starts at 1
	//int pagelen =0;
	vector <Record> record_Buffer;//delete record buffer from header

	struct args_phase1_struct *args = arg;
	args->num_runs = -1;//goes from 0 to n,set to one as the array size is n, else set array size to n+1 to use indexing from 1
	
	//Create and open new file 'file.run_no'
	args->(*run_buffer)[++num_runs] = new File();//file for run1
	char *actual_path;
	sprintf(actual_path,"%s.%d",file_path,num_runs);
	args->(*run_buffer)[num_runs]->Open(0,actual_path);
	
	//***check resets of indexes
	while(args->input->remove(args->temporary)!=0){ // till input pipe is empty
	
		
		//args->(*recordBuffer)[pagelen++] = args->temporary;
	
		//append record temporary to page at pageindex
		if(args->(*buf)[page_Index]->append(args->temporary) == 0){//if page is full
			//run length exceed sort runs and write out run file

			//!!We should not use page or file toBinary methods!!
			//the file is made binary by file.close . When page is full create a new file and append the pages to that file.After the run length exceeds
			//just close the file and open a new file of the next run no.
			
			//add page to file buffer at page_index
			args->(*run_buffer)[num_runs]->AddPage(args->(*buf)[page_Index],page_Index);//getlength doesnt work use page index
			
			//create new run file and empty page buffer	
			if(++page_Index>=args->run_length){//increment if run length is exceeded 
				
				//empty out file into the vector
				//Sort runs vector
				
				//quick sort record buffer array
				
				quicksort(record_Buffer,0,record_Buffer.size(), args->sort_order);

				// record buffer should be sorted now
				
				//new File
				args->(*run_buffer)[++num_runs] = new File();  //create new run file
				char *actual_path;
				sprintf(actual_path,"%s.%d",file_path,num_runs);//set path as "file_path.num_run"
				args->(*run_buffer)[num_runs]->open(0,args->file_path);//??concatenate run no
				
				//empty page buffer ?? do we need to? can we overwrite?
				for(i=0;i<args->run_length;i++)
					args->(*buf)[i]->EmptyItOut();
				
				page_Index=1;
				args->(*buf)[page_Index]->append(args->temporary);
				//code for writing out the run and empting the page buffer
				//USE FILE OBJECT??
				
				Page* ptemp = new Page();
				int j=0;
				int page_index_runs = 0;
				while(true){
					Record* t = record_Buffer.at(j);
					if(ptemp->append(t)!=0)
					{	
						j++;
					}
					else{
						if(j==record_Buffer.size()-1){
							args->(*buff)[num_runs-1]->AddPage(ptemp,page_index_runs++); // add current page to file and increment the index
							ptemp->EmptyItOut();
							ptemp->append(record_Buffer.at(j++));			
						}
						else{
							// last record in vector
							args->(*buff)[num_runs-1]->close();
						
						}
				}
			
			} // page index out of bound

			page_Index = 1;//reset page index
			
		}
		
		//  if page is full, increment page index
		//  if page index is out of bound, sort() page[]
		// write out as page[] run as binary
		// empty put page[]
		
	}
	 
}

BigQ::~BigQ () {
	delete buffer;
	//delete thread
}
