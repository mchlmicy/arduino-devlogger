from django import template
import datetime

register = template.Library()

# Operations
# + Decimal
@register.filter
def add_hours(value, arg):
    print value, arg
    value = value + datetime.timedelta(hours=int(arg))
    return value