<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcubfscctrlcmdexg */

$this->title = 'Create Hcubfscctrlcmdexg';
$this->params['breadcrumbs'][] = ['label' => 'Hcubfscctrlcmdexgs', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfscctrlcmdexg-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
