import operator
from django.db.models import Q
from functools import reduce
from django.shortcuts import render
from django.views import generic
from .models import Package, Truck
from django.contrib.auth.decorators import login_required
from django.contrib.auth.mixins import LoginRequiredMixin
from django.views.generic.edit import UpdateView
from django.urls import reverse_lazy


class PackageListView(LoginRequiredMixin, generic.ListView):
    model = Package

    def get_queryset(self):
        return Package.objects.filter(owner=self.request.user)


class ShipmentListView(generic.ListView):
    model = Package
    template_name_suffix = '_all_list'


class PackageUpdate(LoginRequiredMixin, UpdateView):
    model = Package
    fields = ['xPosition', 'yPosition', 'destRequiredUpdated']
    template_name_suffix = '_update_form'
    success_url = reverse_lazy('upsApp:packages')


class ShipmentSearchListView(ShipmentListView):
    """
    Display a Blog List page filtered by the search query.
    """
    paginate_by = 10

    def get_queryset(self):
        result = super(ShipmentSearchListView, self).get_queryset()

        query = self.request.GET.get('q')
        if query:
            # query_list = query.split()
            result = result.filter(packageId=query)

        return result
