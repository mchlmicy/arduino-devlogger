# App
from app.models import *

# Datatypes
import json
from datetime import date

# Render
from django.shortcuts import render, Http404
from django.http import HttpResponse

# Root
# + Root
def index(request):
    log = Timecard.objects.all()
    context = {'log': log}
    return render(request, 'index.html', context)
# + Data
def data(request):
    if request.is_ajax():
        data = list(Timecard.objects.values_list('duration', 'timestamp_start').order_by('timestamp_start'))
        return HttpResponse(json.dumps(data, default=date_handler), content_type='application/json')
    else:
        raise Http404

# Extra
# + Date Handler
def date_handler(obj):
    return obj.isoformat() if hasattr(obj, 'isoformat') else obj