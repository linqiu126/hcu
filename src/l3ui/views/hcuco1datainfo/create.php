<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuco1datainfo */

$this->title = 'Create Hcuco1datainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuco1datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuco1datainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
