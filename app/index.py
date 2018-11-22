from flask_appbuilder import IndexView, expose
from flask import Response
import pandas as pd

from config import *

class VisulizeIndexView(IndexView):
	index_template = 'index.html'

	@expose('/getTopology/groupA',methods=['GET'])
	def getTopologyA(self):

		table_data = pd.read_csv(CSV_FILE_FOLDER+"/logA.csv",usecols=['from_ip_address','to_ip_address','package_count'])

		return Response(table_data.to_json(orient='records'), mimetype='application/json')

	@expose('/getTopology/groupB',methods=['GET'])
	def getTopologyB(self):

		table_data = pd.read_csv(CSV_FILE_FOLDER+"/logB.csv",usecols=['from_ip_address','to_ip_address','package_count'])

		return Response(table_data.to_json(orient='records'), mimetype='application/json')

