from flask_appbuilder import IndexView, expose
from flask import Response
import pandas as pd

from config import *

class VisulizeIndexView(IndexView):
	index_template = 'index.html'

	@expose('/getTopology',methods=['GET'])
	def getTopology(self):

		table_data = pd.read_csv(CSV_FILE_FOLDER+"/log.csv",usecols=['from_ip_address','to_ip_address','package_count'])

		return Response(table_data.to_json(orient='records'), mimetype='application/json')

