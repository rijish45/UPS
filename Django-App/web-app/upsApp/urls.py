from django.urls import path

from . import views

app_name = 'upsApp'
urlpatterns = [
    path('', views.PackageListView.as_view(), name='packages'),
    path('<int:pk>/update/', views.PackageUpdate.as_view(), name='package_update'),
    path('shipment/', views.ShipmentListView.as_view(), name='shipment'),
    path('shipment/search', views.ShipmentSearchListView.as_view(), name='shipment_search_list_view'),  
]



