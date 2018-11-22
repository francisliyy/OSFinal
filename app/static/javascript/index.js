$(function(){

	var topoChart = echarts.init(document.getElementById('topoChart'));

	var option = {
        title: {
            text: 'Communication Inside 2 Groups of Docker cContainers',
            top: 'bottom',
            left: 'right'
        },
        tooltip: {},
        legend: [{
            data: ['Group A','Group B'],
        }],
        animationDuration: 1500,
        animationEasingUpdate: 'quinticInOut',
        series : [
            {
                name: '',
                type: 'graph',
                layout: 'none',
                data: [],
                links: [],
                categories: [{name:'Group A'},{name:'Group B'}],
                //roam: true,
                focusNodeAdjacency: true,
                itemStyle: {
                    normal: {
                        borderColor: '#fff',
                        borderWidth: 1,
                        shadowBlur: 10,
                        shadowColor: 'rgba(0, 0, 0, 0.3)'
                    }
                },
                label: {
                    position: 'right',
                    formatter: '{b}'
                },
                lineStyle: {
                    color: 'source',
                    curveness: 0
                },
                emphasis: {
                    lineStyle: {
                        width: 10
                    }
                }
            }
        ]
    };
                    

	topoChart.setOption(option);

	$.ajax({
		cache: false,
	  	url: $SCRIPT_ROOT+'/getTopology/groupA',
	  	type: 'GET',
	  	contentType:'application/json',
	  	data: JSON.stringify({}),
	  	success:function(data){
	  		console.log(data);
	  		var nodes = [];
	  		option.series[0].nodes=[];
	  		option.series[0].links=[];
	  		for (let i = 0; i < data.length; i++)
			{
				if(!nodes.includes(data[i]['from_ip_address'])){
					nodes.push(data[i]['from_ip_address']);
				}
				if(!nodes.includes(data[i]['to_ip_address'])){
					nodes.push(data[i]['to_ip_address']);
				}	
				option.series[0].links.push({source:data[i]['from_ip_address'],target:data[i]['to_ip_address'],value:data[i]['package_count'],label:{show:true,formatter:'{b}:{c}'}});		    
			}
			for (let i = 0; i < nodes.length; i++){
				if(i==0){
					option.series[0].data.push({name:nodes[i],x:0,y:5,category:0,label:{show:true}});
				}else if(i%2==0){
					option.series[0].data.push({name:nodes[i],x:0+15,y:5+15*(i-1),category:0,label:{show:true}});
				}else{
					option.series[0].data.push({name:nodes[i],x:0-15,y:5+15*i,category:0,label:{show:true}});
				}
				
			}
			topoChart.setOption(option);
			$.ajax({
				cache: false,
			  	url: $SCRIPT_ROOT+'/getTopology/groupB',
			  	type: 'GET',
			  	contentType:'application/json',
			  	data: JSON.stringify({}),
			  	success:function(data){
			  		console.log(data);
			  		var nodes = [];
			  		for (let i = 0; i < data.length; i++)
					{
						if(!nodes.includes(data[i]['from_ip_address'])){
							nodes.push(data[i]['from_ip_address']);
						}
						if(!nodes.includes(data[i]['to_ip_address'])){
							nodes.push(data[i]['to_ip_address']);
						}	
						option.series[0].links.push({source:data[i]['from_ip_address'],target:data[i]['to_ip_address'],value:data[i]['package_count'],label:{show:true,formatter:'{b}:{c}'}});		    
					}
					for (let i = 0; i < nodes.length; i++){
						if(i==0){
							option.series[0].data.push({name:nodes[i],x:60,y:5,category:1,label:{show:true}});
						}else if(i%2==0){
							option.series[0].data.push({name:nodes[i],x:60+15,y:5+15*(i-1),category:1,label:{show:true}});
						}else{
							option.series[0].data.push({name:nodes[i],x:60-15,y:5+15*i,category:1,label:{show:true}});
						}
						
					}
					topoChart.setOption(option);
					console.log(option);
			  	}
			});
	  	}
	});	


	

});