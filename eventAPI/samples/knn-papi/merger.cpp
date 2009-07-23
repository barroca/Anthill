#include "merger.h"

int merger::merge(AHEvent* event){
	gettimeofday(&merge_btime,NULL);

	// Creating string from the pointer 
	cpair* cpair_buff = (cpair*) event->getData();
	list<cpair> pairs_list;
	for (unsigned int i=0; i<k; i++){
		pairs_list.push_back( cpair_buff[i] );
	}

	double* point = (double*) &cpair_buff[k];
	ostringstream oss;
	oss << "(";
	for (unsigned int i=0;i<d-1;i++){
		oss << point[i] << ",";
	}
	oss << point[d-1] << ")";
	string point_str = oss.str();

	// updating the map
	map<string, list<cpair> >::iterator map_it = points_map.find(point_str);
	if (map_it==points_map.end()){
		pairs_list.resize(k);
		points_map[point_str]=pairs_list;

	}else{
		map_it->second.merge(pairs_list, compare_pair);
		map_it->second.resize(k);
	}

#ifdef DEBUG 
	cout << "Point " <<  point_str << " List: ";
	list<cpair> plist = points_map.find(point_str)->second;
	list<cpair>::iterator it = plist.begin();
	for (; it!=plist.end();it++){
		cout << "("<<(*it).class_id<< "," << (*it).distance << ") " ;
	}
	cout << endl;
#endif
	n_proc++;
	gettimeofday(&merge_etime,NULL);
	cout << fixed << "# " <<  (merge_etime.tv_sec*(double)1000000 + (double)merge_etime.tv_usec)/(double)1000000 << " " << n_proc << endl;
	merge_time += ((merge_etime.tv_sec-merge_btime.tv_sec)*(double)1000000 + (double)(merge_etime.tv_usec-merge_btime.tv_usec))/(double)1000000;
	return 0;
}

provide(merger);
