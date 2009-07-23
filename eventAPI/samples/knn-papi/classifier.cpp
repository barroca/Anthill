#include "classifier.h"

int classifier::trainning(AHEvent* event){
	pthread_mutex_lock(&mutex);
	int class_id;
	memcpy(&class_id, event->getData(), sizeof(int));
	double* dimensions = get_dimensions((double*)&((int*)event->getData())[1],d);

	list<double*> dimension_list;
	map< int, list<double*> >::iterator map_it = train_map.find( class_id );
	if ( map_it == train_map.end() ){ // if the list does not exist create a new one
		train_map[class_id] = dimension_list;
	} else {
		map_it->second.push_back( dimensions );
	}
	number_off_train_reg++;
	pthread_mutex_unlock(&mutex);
//	delete dimensions;
	delete event;
	return 0;
}

int classifier::classify(AHEvent* event){
#ifdef DEBUG
	cout << "CLASSIFIER. Starting classification." << endl;
#endif
	if(a == 0){
		gettimeofday( &btime,NULL );
		a = 1;
//		numCounters = PAPI_SIMPLE_INIT();

	}
	double* point =get_dimensions((double*)&((int*)event->getData())[1],d);
	list<cpair> distances;
	map< int, list<double*> >::iterator map_it = train_map.begin();
	for (; map_it!=train_map.end();map_it++){
		list<double*>::iterator lit = map_it->second.begin();
		for (; lit!=map_it->second.end();lit++){
			cpair p;
			p.class_id = map_it->first;
			p.distance = calculate_distance( point, *lit,d );
			distances.push_back( p );      
		} 
	}

	distances.sort(compare_pair); // this is necessary to the merge function in the merge filter 
	distances.unique(from_the_same_class);

	// Packing data
	unsigned int counter=0;
	const unsigned int buff_size = ( k*sizeof(cpair)) + d*sizeof(double);
	void* buff = malloc( buff_size );

	cpair* cpair_buff = (cpair*) buff;
	list<cpair>::iterator distances_it = distances.begin();

#ifdef DEBUG
	cout << "Filling: ";
	cout << "Point: ( ";
	for (unsigned int i=0;i<d;i++){
		cout << point[i] << " ";
	}
	cout << ")  K Nearest Distances (classId, distance): " ;
#endif

	for(;distances_it!=distances.end() && counter < k;distances_it++,counter++){
		cpair_buff[counter] = *distances_it;
#ifdef DEBUG
		cout << "("<< cpair_buff[counter].class_id <<","<< cpair_buff[counter].distance<<") " ; 
#endif      
	}

	// filling with big values 
	cpair nothing;
	nothing.class_id = 0;
	nothing.distance = (double)LONG_MAX;
	for (; counter<k; counter++){
		cpair_buff[counter] = nothing;

#ifdef DEBUG
		cout << "("<< cpair_buff[counter].class_id <<","<< cpair_buff[counter].distance<<") " ; 
#endif      
	}

#ifdef DEBUG
	cout << endl;
#endif      

	memcpy( &cpair_buff[counter], point,(d*sizeof(double))  );

	AHEvent *sent_event = new AHEvent(buff, buff_size, classifier_to_merger);
	dispachEvent(sent_event);
	//   pthread_mutex_lock(&test_mutex);
	//   threadsInFunction--;
	//   pthread_mutex_unlock(&test_mutex);

	for(;distances_it!=distances.end() && counter < k;distances_it++,counter++){
		
		free((void*)&(*distances_it));
	}
#ifdef DEBUG
	cout << "Point: ( ";
	for (unsigned int i=0;i<d;i++){
		cout << point[i] << " ";
	}
	cout << ")  K Nearest Distances (classId, distance): " ;
	for(unsigned int i=0;i<k;i++){
		cout << "( "<< cpair_buff[i].class_id <<","<< cpair_buff[i].distance<<" ) " ; 
	}
	cout << endl;
#endif
	delete event;
	free(point);

	cout << "END CLASSIFIER"<<endl;
//	 PAPI_SIMPLE_PRINT(numCounters, "CLASSIFIER_CACHE");
	return 0;
}

void classifier::print_train_map(){
   cout << "Train map: "<<endl;
   map< int, list<double*> >::iterator map_it = train_map.begin();
   for (; map_it!=train_map.end();map_it++){
      cout << "  ClassID: " << map_it->first << endl;
      list<double*>::iterator lit = map_it->second.begin();
      for (; lit!=map_it->second.end();lit++){
         cout << "Point: ";
         for (unsigned int i=0; i<d; i++){
            cout << (*lit)[i] << " ";
         }
         cout << endl;
      } 
   }
}


provide(classifier) /* Macro that registers the filter don't touch here*/
