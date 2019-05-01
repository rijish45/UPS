from django.db import models

# Create your models here.

from django.contrib.auth.models import User

class Truck(models.Model):
    truckId = models.PositiveIntegerField(blank = False, primary_key = True)
    STATUS_CHOICE = (
        ('ID', 'idle'),
        ('TR', 'traveling'),
        ('AW', 'arrive warehouse'),
        ('LO', 'loading'),
        ('DE', 'delivering'),
    )
    status = models.CharField (
        max_length = 20, 
        choices = STATUS_CHOICE, 
        default = 'ID',
    )
    def __str__(self):
        return "the truckId of is package is %s" % (self.truckId)


class Package(models.Model):
    packageId = models.PositiveIntegerField(blank = False,  primary_key = True)
    xPosition = models.IntegerField(default = 0, blank = False, null = True)
    yPosition = models.IntegerField(default = 0, blank = False, null = True)
    item = models.CharField(max_length = 200, blank = True, null = True)
    whid = models.PositiveIntegerField(default = 0, blank = False)
    whxPosition = models.IntegerField(default = 0, blank = False)
    whyPosition = models.IntegerField(default = 0, blank = False)
    destRequiredUpdated = models.BooleanField(default = False)
    owner = models.ForeignKey (
        User,
        related_name = 'owner_package_set',
        on_delete = models.SET_NULL,
        blank = True,
        null = True,
    )
    truck = models.ForeignKey (
        Truck, 
        blank = True,
        null = True,
        on_delete = models.SET_NULL,
        related_name = 'truck_package_set'
    )
    STATUS_CHOICE = (
        ('LG', 'loading'),
        ('LD', 'loaded'),
        ('DG', 'delivering'),
        ('DD', 'delivered'),
    )
    status = models.CharField (
        max_length = 20, 
        choices = STATUS_CHOICE, 
        default = 'DG',
    )
    def __str__(self):
        return "the packageId of is package is %s" % (self.packageId)



class current_world(models.Model):
    name = models.CharField(max_length=30,null=True,default="current_world")
    worldid = models.CharField(max_length=30,null=True)


class sequence_number(models.Model):
    sequence_id = models.PositiveIntegerField(blank = False, primary_key = True)
    ackedornot = models.BooleanField(default = False)

class order_table(models.Model):
    order_id = models.PositiveIntegerField(blank = False, primary_key = True);
    package_num = models.OneToOneField(
        Package,
        on_delete = models.CASCADE,
        unique = True
    )
