# URLs
from django.conf.urls import patterns, url

# Views
from app import views

urlpatterns = patterns('',

    # log
    url(r'^$', views.index, name='index'), # / (index)
    url(r'^data/$', views.data, name='data'), # data/ (data)
)
