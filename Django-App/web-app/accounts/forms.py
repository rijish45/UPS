from django.contrib.auth.models import User
from django import forms

class UserForm(forms.ModelForm):
    password = forms.CharField(widget = forms.PasswordInput)

    class Meta:
        model = User
        fields = ['username',  'password']
        help_texts = {
            'username': ('Please create your username'),
        }

    def clean_password(self):
        password = self.cleaned_data.get('password')

        if len(password) < 6:
            raise forms.ValidationError("Your password is too short.")
        elif len(password) > 20:
            raise forms.ValidationError("Your password is too long.")

        return password