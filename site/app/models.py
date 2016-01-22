from django.db import models

# Datatypes
from site_packages import timezone as timezone

# Timecard
class Timecard(models.Model):
    duration = models.IntegerField() # duration in seconds
    timestamp_start = models.DateTimeField() # timestamp_start
    timestamp_end = models.DateTimeField() # timestamp_end
    def __unicode__(self):
        return u"%s" % self.id
