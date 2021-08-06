import { AlertOptions } from '@ionic/angular';

export const displayAlert = async (header: string, message: string) => {
  const alert = document.createElement('ion-alert');

  const options: AlertOptions = {
    header,
    message,
    buttons: ['OK'],
  };

  Object.assign(alert, options);

  document.body.appendChild(alert);
  await alert.present();
};
