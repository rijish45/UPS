from django.shortcuts import render
from django.urls import reverse_lazy
from django.views import generic
from django.contrib.auth import login, authenticate
from .forms import UserForm
from django.contrib.auth.models import User
from django.shortcuts import render, redirect



def signup(request):
	if request.method == 'POST':
		form = UserForm(request.POST)
		if form.is_valid():
			newUser = User.objects.create(
				username = form.cleaned_data.get('username'),
			)
			newUser.set_password(form.cleaned_data.get('password'))
			newUser.save()              
			tempUser = authenticate(
				username = form.cleaned_data.get('username'), 
				password = form.cleaned_data.get('password'),
			)
			login(request, tempUser)
			return redirect('/') # direct to the home page
	else:
		form = UserForm()
	return render(request, 'registration/signup.html', {'form': form})



