$(function(){

	var topoChart = echarts.init(document.getElementById('topoChart'));

	var option = {
        title: {
            text: 'Les Miserables',
            subtext: 'Default layout',
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
                name: 'Les Miserables',
                type: 'graph',
                layout: 'none',
                data: [{name:'192.168.0.1',x:10,y:20,label:{show:true}},{name:'192.168.0.2',x:20,y:20,label:{show:true}}],
                links: [{source:'192.168.0.1',target:'192.168.0.2',value:'100',label:{show:true}}],
                //categories: ,
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
                    curveness: 0.3
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

	console.log("hello begin");

	$.ajax({
		cache: false,
	  	url: $SCRIPT_ROOT+'/getTopology',
	  	type: 'GET',
	  	contentType:'application/json',
	  	data: JSON.stringify({}),
	  	success:function(data){
	  		console.log(data);
	  	}
	});

	console.log("hello end");
	

});